
/*
 *  Copyright (c) 2014, Nicolas Danet, 
 *  "nicolas.danet@free.fr".
 *
 *  Permission is hereby granted, free of charge, to any person obtaining
 *  a copy of this software and associated documentation files (the
 *  "Software"), to deal in the Software without restriction, including
 *  without limitation the rights to use, copy, modify, merge, publish,
 *  distribute, sublicense, and/or sell copies of the Software, and to
 *  permit persons to whom the Software is furnished to do so, subject to
 *  the following conditions:
 *
 *  The above copyright notice and this permission notice shall be
 *  included in all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 *  LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 *  OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 *  WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------

/* ValueTree and UndoManager basics. */

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

namespace JojoId
{
    #define JOJO_ID(name)   const Identifier name(#name)
    
    JOJO_ID(JojoTree);
    
    #undef JOJO_ID
}

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------

struct _jojo;

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
#pragma mark -

/* ValueTree callbacks are synchronous. */

class Oizo : public ValueTree::Listener, private Timer {                /* Timer for convenience only. */

public:
    explicit Oizo(_jojo* o) : owner(o) { startTimer(500); }

public:
    void valueTreePropertyChanged(ValueTree& tree, const Identifier& property) { 
    //
    post("Changed / %s", property.toString( ).toRawUTF8( ));
    //
    }
    
    void valueTreeChildAdded(ValueTree&, ValueTree&)    { }
    void valueTreeChildRemoved(ValueTree&, ValueTree&)  { }
	void valueTreeChildOrderChanged(ValueTree&)         { }
    void valueTreeParentChanged (ValueTree&)            { }
    void valueTreeRedirected(ValueTree&)                { }

private: 
    struct _jojo* owner; 
    
    void timerCallback( );
    
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Oizo)
};

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
#pragma mark -

typedef struct _jojo {

public :
    _jojo( ) : mUndo(new UndoManager( )), mOizo(new Oizo(this)), mTree(new ValueTree(JojoId::JojoTree)) { 
    //
    mTree->addListener(mOizo);
    //
    }
    
    ~_jojo( ) { mTree->removeListener(mOizo); }     /* Currently not necessary, but for future! */

public:
    t_object                    ob;
    ulong                       mError;
    ScopedPointer<UndoManager>  mUndo;
    ScopedPointer<Oizo>         mOizo;
    ScopedPointer<ValueTree>    mTree;
                     
    } t_jojo;

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
#pragma mark -

void Oizo::timerCallback( ) 
{ 
    owner->mUndo->beginNewTransaction( ); 
}

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

void jojo_quit(void);
void jojo_quit(void)
{
    shutdownJuce_GUI( ); cpost("Shutdown JUCE\n");
}

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
#pragma mark -

#define JOJO_INITIALIZE \
    {   \
    initialiseJuce_GUI( );   \
    cpost("Initialize JUCE\n"); \
    quittask_install((method)jojo_quit, NULL);  \
    }
    
// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
#pragma mark -

void *jojo_new      (t_symbol *s, long argc, t_atom *argv);
void jojo_free      (t_jojo *x);
void jojo_bang      (t_jojo *x);
void jojo_undo      (t_jojo *x);
void jojo_redo      (t_jojo *x);
void jojo_anything  (t_jojo *x, t_symbol *s, long argc, t_atom *argv);

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
#pragma mark -

static t_class *jojo_class;

JOJO_EXPORT int main(void)
{   
    t_class *c = NULL;
    
    c = class_new("jojoTree", (method)jojo_new, (method)jojo_free, sizeof(t_jojo), NULL, A_GIMME, 0);
    
    class_addmethod(c, (method)jojo_bang,       "bang", 0);
    class_addmethod(c, (method)jojo_undo,       "undo", 0);
    class_addmethod(c, (method)jojo_redo,       "redo", 0);
    class_addmethod(c, (method)jojo_anything,   "anything", A_GIMME, 0);
    
    class_register(CLASS_BOX, c);
    jojo_class = c;
    
    JOJO_INITIALIZE
    
    return 0;
}

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
#pragma mark -

void *jojo_new(t_symbol *s, long argc, t_atom *argv)
{
    t_jojo *x = NULL;
    
    if ((x = (t_jojo *)object_alloc(jojo_class))) {
    //
    ulong err = (x->mError = JOJO_GOOD);
    
    try {
        new(x)t_jojo;
    }
    
    catch (...) {
        err = (x->mError = JOJO_ERROR);
    }
    
    if (err) {
        object_free(x);
        x = NULL;
    }
    //
    }
    
    return x;
}

void jojo_free(t_jojo *x)
{
    if (!x->mError) { x->~t_jojo( ); }
}

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
#pragma mark -

/* Best to keep everything in the main thread. */

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------

void jojo_bang(t_jojo *x) 
{
    if (systhread_ismainthread( )) {        
        post("%s", x->mTree->toXmlString( ).toRawUTF8( ));
    }
}

void jojo_undo(t_jojo *x) 
{
    if (systhread_ismainthread( )) {        
        post("Undo / %ld", x->mUndo->undo( ));
    }
}

void jojo_redo(t_jojo *x) 
{
    if (systhread_ismainthread( )) {
        post("Redo / %ld", x->mUndo->redo( ));
    }
}

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------

void jojo_anything(t_jojo *x, t_symbol *s, long argc, t_atom *argv)
{
    if (!systhread_ismainthread( )) { error("Always in the main thread!"); }        
    else if (argc) {
    //
    const String property(s->s_name);
    
    if (Identifier::isValidIdentifier(property)) {
    //
    if (atom_gettype(argv) == A_SYM) { 
        x->mTree->setProperty(property, atom_getsym(argv)->s_name, x->mUndo);
    } else {
        x->mTree->setProperty(property, atom_getfloat(argv), x->mUndo);
    }
    //
    }
    //
    }
}

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------