
/* 
    Copyright (c) 2014, Nicolas Danet, < nicolas.danet@free.fr >.

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

/* "Is-a" DynamicObject (probably not a good idea) / Clone part. */

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

class Oizo;

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------

typedef ReferenceCountedObjectPtr < Oizo > OizoPtr;

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
#pragma mark -

class Oizo : public DynamicObject {

public:
    Oizo() : eggs_() { post ("Oizo ctor"); }
    ~Oizo() { post ("Oizo dtor"); }
    
public:
    DynamicObject::Ptr clone() const { return new Oizo (*this); }

    /* Spawn method to be set dynamically. */
    
    static var spawn (const var::NativeFunctionArgs& args) {
        if (Oizo* o = dynamic_cast < Oizo* > (args.thisObject.getObject())) { o->doSpawn(); } 
        return var::undefined();
    }

private:
    void doSpawn() { ++eggs_; post ("Great, %ld eggs!", eggs_.get()); }
    
    /* Copy constructor is used in the clone method. */
    
    Oizo (const Oizo& o) : DynamicObject (o), eggs_ (12) { cloneAllProperties(); post ("Oizo copy"); } 
    Oizo& operator = (const Oizo&);

private:
    Atomic < int > eggs_;
    
private:
    JUCE_LEAK_DETECTOR (Oizo)
};

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
#pragma mark -

typedef struct _jojo {

public:
    _jojo() : oizo_ (new Oizo()), lock_() { 
        oizo_->setProperty (JojoIdentifier::One, "Carotte");         /* The DynamicObject stuff. */
        oizo_->setMethod (JojoIdentifier::Two, Oizo::spawn);
    }

public:
    t_object ob_;
    ulong error_;
    OizoPtr oizo_;
    CriticalSection lock_;      /* DynamicObject is not thread-safe. */
    
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
void jojo_spawn (t_jojo *x);
void jojo_clone (t_jojo *x);

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
#pragma mark -

static t_class *jojo_class;

JOJO_EXPORT int main (void)
{   
    t_class *c = NULL;
    
    c = class_new ("jojoObject", (method)jojo_new, (method)jojo_free, sizeof (t_jojo), NULL, A_GIMME, 0);
    
    class_addmethod (c, (method)jojo_bang,  "bang",     0);
    class_addmethod (c, (method)jojo_spawn, "spawn",    0);
    class_addmethod (c, (method)jojo_clone, "clone",    0);
    
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
    
    post ("Var / %s", x->oizo_->getProperty (JojoIdentifier::One).toString().toRawUTF8());
}

void jojo_spawn (t_jojo *x)
{
    const ScopedLock myLock (x->lock_);

    /* Arguments set to zero for convenience. */
    
    x->oizo_->invokeMethod (JojoIdentifier::Two, var::NativeFunctionArgs (x->oizo_.getObject(), nullptr, 0));
}

void jojo_clone (t_jojo *x)
{
    const ScopedLock myLock (x->lock_);
    
    OizoPtr o = x->oizo_->clone();
    o->invokeMethod (JojoIdentifier::Two, var::NativeFunctionArgs (o.getObject(), nullptr, 0));
}

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
