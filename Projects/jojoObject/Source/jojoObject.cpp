
/* 
    { LICENSE }
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
    #define JOJO_ID(name)   const Identifier name(#name)
    
    JOJO_ID(One);
    JOJO_ID(Two);
    
    #undef JOJO_ID
}

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
#pragma mark -

class Oizo;

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------

typedef ReferenceCountedObjectPtr<Oizo> OizoPtr;

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
#pragma mark -

class Oizo : public DynamicObject {

public:
    explicit Oizo() : eggs()    { post("Oizo ctor"); }
    ~Oizo()                     { post("Oizo dtor"); }
    
public:
    DynamicObject::Ptr clone() const { return new Oizo(*this); }

    /* Spawn method to be set dynamically. */
    
    static var spawn(const var::NativeFunctionArgs& args) {
        if (Oizo* o = dynamic_cast<Oizo*>(args.thisObject.getObject())) { o->doSpawn(); } 
        return var::undefined();
    }

private:
    void doSpawn() { ++eggs; post("Great, %ld eggs!", eggs.get()); }
    
    /* Copy constructor is used in the clone method. */
    
    Oizo(const Oizo& o) : DynamicObject(o), eggs(12) { cloneAllProperties(); post("Oizo copy"); } 
    Oizo& operator = (const Oizo&);

    Atomic<int> eggs;
    
private:
    JUCE_LEAK_DETECTOR(Oizo)
};

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
#pragma mark -

typedef struct _jojo {

public :
    _jojo() : mOizo(new Oizo()), mLock() { 
        mOizo->setProperty(JojoIdentifier::One, "Carotte");         /* The DynamicObject stuff. */
        mOizo->setMethod(JojoIdentifier::Two, Oizo::spawn);
    }

public:
    t_object        ob;
    ulong           mError;
    OizoPtr         mOizo;
    CriticalSection mLock;      /* DynamicObject is not thread-safe. */
    
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

JOJO_EXPORT int main(void)
{   
    t_class *c = NULL;
    
    c = class_new("jojoObject", (method)jojo_new, (method)jojo_free, sizeof(t_jojo), NULL, A_GIMME, 0);
    
    class_addmethod(c, (method)jojo_bang,   "bang",     0);
    class_addmethod(c, (method)jojo_spawn,  "spawn",    0);
    class_addmethod(c, (method)jojo_clone,  "clone",    0);
    
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
    if (!x->mError) { x->~t_jojo(); }
}

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
#pragma mark -

void jojo_bang(t_jojo *x)
{
    const ScopedLock myLock(x->mLock);
    
    post("Var / %s", x->mOizo->getProperty(JojoIdentifier::One).toString().toRawUTF8());
}

void jojo_spawn(t_jojo *x)
{
    const ScopedLock myLock(x->mLock);

    /* Arguments set to zero for convenience. */
    
    x->mOizo->invokeMethod(JojoIdentifier::Two, var::NativeFunctionArgs(x->mOizo.getObject(), nullptr, 0));
}

void jojo_clone(t_jojo *x)
{
    const ScopedLock myLock(x->mLock);
    
    OizoPtr o = x->mOizo->clone();
    o->invokeMethod(JojoIdentifier::Two, var::NativeFunctionArgs(o.getObject(), nullptr, 0));
}

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
