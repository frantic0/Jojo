
/* 
    { LICENSE }
*/

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------

/* Javascript's "Hello World!". */

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

class Oizo : public DynamicObject {

public:
    explicit Oizo()    { setMethod ("hello", Oizo::hello); post ("Oizo ctor"); }
    ~Oizo()            { post ("Oizo dtor"); }

    static Identifier getClassName() { static const Identifier i ("Oizo"); return i; }
    
public:

    /* Custom method to be added to the DynamicObject. */
    
    static var hello (const var::NativeFunctionArgs& args) {
    //
    post ("Hello World!");
    
    if (dynamic_cast<Oizo*> (args.thisObject.getObject())) {             /* Class call. */
    //
    post ("Class / %s", args.thisObject.toString().toRawUTF8());
    //
    } else {                                                            /* Prototype call. */
    //
    post ("Prototype / %s", args.thisObject.toString().toRawUTF8());
    post ("From / %s", args.thisObject.getDynamicObject()->getProperty ("prototype").toString().toRawUTF8());
    //
    }
        
    return var::undefined();
    //
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Oizo)
};

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------

#pragma mark -

typedef struct _jojo {

public :
    _jojo() : mJS (new JavascriptEngine()), mLock() { 
        mJS->registerNativeObject (Oizo::getClassName(), new Oizo());      /* Takes Oizo's ownership. */
    }

public:
    t_object ob;
    ulong mError;
    ScopedPointer <JavascriptEngine> mJS;
    CriticalSection mLock;
    
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
    
    c = class_new ("jojoJavascript", (method)jojo_new, (method)jojo_free, sizeof (t_jojo), NULL, A_GIMME, 0);
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

void jojo_bang (t_jojo *x)
{
    const ScopedLock myLock (x->mLock);      /* Javacript engine is not thread-safe. */

    const Result result (x->mJS->execute ("var o = new Oizo(); Oizo.hello(); o.hello();"));
        
    if (result.wasOk()) { }
    else {
        post ("%s", result.getErrorMessage().toRawUTF8());
    }
}

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
