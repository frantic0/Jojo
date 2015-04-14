
/* 
    Copyright (c) 2014, Nicolas Danet, < nicolasdanet@free.fr >.

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
    THE SOFTWARE.

*/

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------

/* PropertiesFile experiments. */

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------

/* Due to the message manager loop, thread-safety needs extra care. */

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------

#include <Carbon/Carbon.h>
#include <QuickTime/QuickTime.h>

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------

#include "ext.h"
#include "ext_obex.h"

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------

#include "ext_systhread.h"

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------

#include "JuceHeader.h"

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
#pragma mark -

class Oizo : public ChangeListener {

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
#pragma mark -

public:
    Oizo()  { cpost ("Oizo ctor\n"); }
    ~Oizo() { cpost ("Oizo dtor\n"); }

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
#pragma mark -

public:
    void changeListenerCallback (ChangeBroadcaster*) { post ("Something have changed!"); }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Oizo)
};

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
#pragma mark -

typedef struct _jojo {

public:
    _jojo() : oizo_ (new Oizo()), properties_ (nullptr) { 
    //
    /* File is next to the bundle for convenience only. */
    
    File folder (File::getSpecialLocation (File::currentApplicationFile));
    File settings (folder.getSiblingFile ("jojoProperties.txt"));
    properties_ = new PropertiesFile (settings, PropertiesFile::Options());
    properties_->addChangeListener (oizo_);
    //
    }
    
    ~_jojo() { properties_->removeChangeListener (oizo_); properties_->saveIfNeeded(); }
    
public:
    t_object ob_;
    ulong error_;
    ScopedPointer < Oizo > oizo_;
    ScopedPointer < PropertiesFile > properties_;   /* Consider to use ApplicationProperties in your code. */
    
    } t_jojo;
    
// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
#pragma mark -

#ifdef WIN_VERSION
    #define JOJO_EXPORT __declspec(dllexport)
#else
    #define JOJO_EXPORT __attribute__((visibility("default")))
#endif

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
#pragma mark -

#define JOJO_GOOD   0UL
#define JOJO_ERROR  1UL

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------

#include <memory>

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
#pragma mark -

void jojo_quit (void);
void jojo_quit (void)
{
    shutdownJuce_GUI(); cpost ("Shutdown JUCE\n");
}

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
#pragma mark -

#define JOJO_INITIALIZE \
    {   \
    initialiseJuce_GUI();   \
    cpost ("Initialize JUCE\n"); \
    quittask_install ((method)jojo_quit, NULL);  \
    }
    
// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
#pragma mark -

void *jojo_new      (t_symbol *s, long argc, t_atom *argv);
void jojo_free      (t_jojo *x);
void jojo_bang      (t_jojo *x);
void jojo_anything  (t_jojo *x, t_symbol *s, long argc, t_atom *argv);

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
#pragma mark -

static t_class *jojo_class;

JOJO_EXPORT int main (void)
{   
    t_class *c = NULL;
    
    c = class_new ("jojoProperties", (method)jojo_new, (method)jojo_free, sizeof (t_jojo), NULL, A_GIMME, 0);
    
    class_addmethod (c, (method)jojo_bang,      "bang",     0);
    class_addmethod (c, (method)jojo_anything,  "anything", A_GIMME, 0);
    
    class_register (CLASS_BOX, c);
    jojo_class = c;
    
    JOJO_INITIALIZE         /* Needed to initialize the message manager. */
    
    return 0;
}

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
#pragma mark -

void *jojo_new (t_symbol *s, long argc, t_atom *argv)
{
    t_jojo *x = NULL;
    
    if ((x = (t_jojo *)object_alloc (jojo_class))) {
    //
    ulong err = (x->error_ = JOJO_GOOD);
    
    try {
        new (x) t_jojo;
    }
    
    catch (...) {
        err = (x->error_ = JOJO_ERROR);
    }

    if (err) {
        object_free (x);
        x = NULL;
    }
    //
    }
    
    return x;
}

void jojo_free (t_jojo *x)
{
    if (!x->error_) { x->~t_jojo(); }
}

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
#pragma mark -

void jojo_bang (t_jojo *x) 
{
    /* To avoid locking mess manage everything in the main thread. */
    
    if (!systhread_ismainthread()) { error ("Always in the main thread!"); } 
    else {
    //
    // const ScopedLock lock (x->properties_->getLock());
    post ("Keys / %s", x->properties_->getAllProperties().getAllKeys().joinIntoString (" / ").toRawUTF8());
    post ("Values / %s", x->properties_->getAllProperties().getAllValues().joinIntoString (" / ").toRawUTF8());
    //
    }
}

void jojo_anything (t_jojo *x, t_symbol *s, long argc, t_atom *argv)
{
    if (!systhread_ismainthread()) { error ("Always in the main thread!"); } 
    else {
    //
    if (argc) {
        if (atom_gettype (argv) == A_SYM) { 
            x->properties_->setValue (s->s_name, atom_getsym (argv)->s_name); 
        } else {
            x->properties_->setValue (s->s_name, atom_getfloat (argv)); 
        }
    } 
    //
    }
}

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
