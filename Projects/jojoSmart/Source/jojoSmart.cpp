
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

/* Is that pointers are smart? */

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

/* Custom deleter for ScopedPointer (shared_ptr like workaround). */
 
// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
#pragma mark -

class Oizo {

friend class ContainerDeletePolicy<Oizo>;

public:
    Oizo( )     { post("Oizo ctor"); }

private:
    ~Oizo( )    { post("Oizo dtor"); }
};

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
#pragma mark -

namespace juce {

template <> struct ContainerDeletePolicy<Oizo> {

static void destroy (Oizo* o) { post("Oizo policy"); delete o; }

};

} // namespace juce

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------

/* Reference counting. */

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
#pragma mark -

class Kitty : public ReferenceCountedObject {

public:
    Kitty( )  { post("Kitty ctor"); }
    ~Kitty( ) { post("Kitty dtor"); }
    
    void doSomething( ) const { post("Kitty do something very fun!"); }
};

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
#pragma mark -

typedef ReferenceCountedObjectPtr<Kitty> KittyPtr;

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------

/* WeakReference (caution: it doesn't retain the object like weak_ptr). */

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
#pragma mark -

class Felix {

friend class WeakReference<Felix>;

public:
    Felix( )     { post("Felix ctor"); }
    ~Felix( )    { post("Felix dtor"); masterReference.clear(); }

private:
    WeakReference<Felix>::Master masterReference;
};

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
#pragma mark -

typedef struct _jojo {

public :
    _jojo( ) : mOizo(new Oizo( )) { }

public:
    t_object            ob;
    ulong               mError;
    ScopedPointer<Oizo> mOizo;
    
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

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
#pragma mark -

static t_class *jojo_class;

JOJO_EXPORT int main(void)
{   
    t_class *c = NULL;
    
    c = class_new("jojoSmart", (method)jojo_new, (method)jojo_free, sizeof(t_jojo), NULL, A_GIMME, 0);
    class_addmethod(c, (method)jojo_bang, "bang", 0);
    class_register(CLASS_BOX, c);
    jojo_class = c;
    
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

void jojo_bang(t_jojo *x)
{
    /* Avoid to accidentally delete a raw pointer owned by a ScopedPointer. */
    
    //delete x->mOizo.get( );       /* Error: 'Oizo::~Oizo()' is private! */
    
    /* */
    
    KittyPtr ptrA(new Kitty( ));
    KittyPtr ptrB(ptrA);  
    
    ptrA->doSomething( );
    ptrB->doSomething( );
    
    ReferenceCountedArray<Kitty> RCArray;
    
    RCArray.add(ptrA);
    RCArray.add(ptrB);
    
    ptrA = nullptr;
    ptrB = nullptr;
    
    post("?"); RCArray.clear( ); post("!");
    
    /* */
    
    Felix* n = new Felix( );
    WeakReference<Felix> myObjectRef = n;

    Felix* p = myObjectRef; post("%ld", p == nullptr);
    delete n;
    post("%ld", p == nullptr);  /* Caution: pointer is no more valid! */
    
    Felix* q = myObjectRef; post("%ld", q == nullptr);
}

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
