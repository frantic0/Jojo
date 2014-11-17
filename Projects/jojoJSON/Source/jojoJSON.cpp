
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

/* "Is-a" DynamicObject / JSON part. */

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

#include "JuceHeader.h"

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
#pragma mark -

namespace JojoIdentifier
{
    #define JOJO_ID(name)   const Identifier name (#name)
    
    JOJO_ID (One);
    JOJO_ID (Two);
    
    #undef JOJO_ID
}

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
#pragma mark -

class Oizo : public DynamicObject {

public:
    Oizo()  { post ("Oizo ctor"); }
    ~Oizo() { post ("Oizo dtor"); }

public:
    void writeAsJSON (OutputStream& stream, int indentLevel, bool allOnOneLine) {
        
        /* Placeholder for future JSON fun! */
        
        DynamicObject::writeAsJSON (stream, indentLevel, allOnOneLine);  /* Call base class function. */
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Oizo)
};

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------

typedef ReferenceCountedObjectPtr < Oizo > OizoPtr;

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
#pragma mark -

typedef struct _jojo {

public:
    _jojo() : oizo_ (new Oizo()), lock_() { 
        oizo_->setProperty (JojoIdentifier::One, "Carotte");
        oizo_->setProperty (JojoIdentifier::Two, "Olive");
    }

public:
    t_object ob_;
    ulong error_;
    OizoPtr oizo_;
    CriticalSection lock_;
    
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

void *jojo_new  (t_symbol *s, long argc, t_atom *argv);
void jojo_free  (t_jojo *x);
void jojo_bang  (t_jojo *x);
void jojo_toto  (t_jojo *x);

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------

void jojo_write (t_jojo *x, const File& aFile);
void jojo_read  (t_jojo *x, const File& aFile);

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
#pragma mark -

static t_class *jojo_class;

JOJO_EXPORT int main (void)
{   
    t_class *c = NULL;
    
    c = class_new ("jojoJSON", (method)jojo_new, (method)jojo_free, sizeof (t_jojo), NULL, A_GIMME, 0);
    
    class_addmethod (c, (method)jojo_bang, "bang", 0);
    class_addmethod (c, (method)jojo_toto, "toto", 0);
    
    class_register (CLASS_BOX, c);
    jojo_class = c;
    
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
    const ScopedLock myLock (x->lock_); 
    
    File jsonFile ((File::getSpecialLocation (File::currentApplicationFile)).getSiblingFile ("jojoJSON.txt"));
    
    jojo_write (x, jsonFile);    /* Write and read a Variant with a DynanmicObject. */
    jojo_read (x, jsonFile); 
}

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
#pragma mark -

void jojo_write (t_jojo *x, const File& aFile)
{
    FileOutputStream outputStream (aFile);
    
    if (outputStream.openedOk()) { 
    //
    var test ("Le sentier longeait la falaise");
    test.append ("Lapin");
    test.append ("Fusil");
    test.append (3.14);
    test.append (x->oizo_.getObject());
    
    outputStream.setPosition (0); 
    outputStream.truncate();
    JSON::writeToStream (outputStream, test);
    //
    }
}

void jojo_read (t_jojo *x, const File& aFile)
{
    FileInputStream inputStream (aFile);
        
    if (inputStream.openedOk()) {
    // 
    var test (JSON::parse (inputStream));
    
    if (test.isArray()) {      /* Should be true in that stupid example! */
    //
    for (int i = 0; i < test.size(); ++i) {
        post ("? / %s", test[i].toString().toRawUTF8());
        if (DynamicObject::Ptr o = test[i].getDynamicObject()) {
            for (int j = 0; j < o->getProperties().size(); ++j) {
                post ("? / --- %s", o->getProperties().getName(j).toString().toRawUTF8());   // --
            }
        }
    }
    //
    }
    //
    }
}

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
#pragma mark -

void jojo_toto (t_jojo *x)
{
    var test (new Oizo());
    test.getDynamicObject()->setProperty (JojoIdentifier::One, new Oizo());
    test.getDynamicObject()->setProperty (JojoIdentifier::Two, new Oizo());
    
    post ("%s", JSON::toString (test).toRawUTF8());
}

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
