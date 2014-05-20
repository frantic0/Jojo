
/* 
    { LICENSE }
*/

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------

/* Is that pointer is smart? */

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

friend class ContainerDeletePolicy <Oizo>;

public:
    explicit Oizo()    { post ("Oizo ctor"); }

private:
    ~Oizo()            { post ("Oizo dtor"); }
};

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
#pragma mark -

namespace juce {

template <> struct ContainerDeletePolicy <Oizo> {

static void destroy (Oizo* o) { post ("Oizo policy"); delete o; }

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
    explicit Kitty()   { post ("Kitty ctor"); }
    ~Kitty()           { post ("Kitty dtor"); }
    
    void doSomething() const { post ("Kitty do something very fun!"); }
};

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
#pragma mark -

typedef ReferenceCountedObjectPtr <Kitty> KittyPtr;

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------

/* WeakReference (caution: it doesn't retain the object like weak_ptr). */

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
#pragma mark -

class Felix {

friend class WeakReference <Felix>;

public:
    explicit Felix()   { post ("Felix ctor"); }
    ~Felix()           { post ("Felix dtor"); masterReference.clear(); }

private:
    WeakReference <Felix>::Master masterReference;
};

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
#pragma mark -

typedef struct _jojo {

public :
    _jojo() : mOizo (new Oizo()) { }

public:
    t_object ob;
    ulong mError;
    ScopedPointer <Oizo> mOizo;
    
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

JOJO_EXPORT int main (void)
{   
    t_class *c = NULL;
    
    c = class_new ("jojoSmart", (method)jojo_new, (method)jojo_free, sizeof (t_jojo), NULL, A_GIMME, 0);
    class_addmethod (c, (method)jojo_bang, "bang", 0);
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
    ulong err = (x->mError = JOJO_GOOD);
    
    try {
        new (x) t_jojo;
    }
    
    catch (...) {
        err = (x->mError = JOJO_ERROR);
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
    if (!x->mError) { x->~t_jojo(); }
}

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
#pragma mark -

void jojo_test (const KittyPtr& temporary)
{
    (void)temporary;
}

void jojo_bang (t_jojo *x)
{
    /* Avoid to accidentally delete a raw pointer owned by a ScopedPointer. */
    
    // delete x->mOizo.get();       /* Error: 'Oizo::~Oizo()' is private! */
    
    KittyPtr ptrA (new Kitty());
    KittyPtr ptrB (ptrA);  
    
    ptrA->doSomething();
    ptrB->doSomething();
    
    ReferenceCountedArray <Kitty> RCArray;
    
    RCArray.add (ptrA);
    RCArray.add (ptrB);
    
    ptrA = nullptr;
    ptrB = nullptr;
    
    post ("?"); RCArray.clear(); post ("!");
    
    //
    
    Felix* n = new Felix();
    WeakReference <Felix> myObjectRef = n;

    Felix* p = myObjectRef; post ("%ld", p == nullptr);
    delete n;
    post ("%ld", p == nullptr);     /* Caution: pointer is no more valid! */
    
    Felix* q = myObjectRef; post ("%ld", q == nullptr);
    
    post ("Temporary");
    jojo_test (new Kitty());    /* Properly freed? */
}

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
