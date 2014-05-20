
/* 
    { LICENSE }
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
    explicit Oizo()    { post ("Oizo ctor"); }
    ~Oizo()            { post ("Oizo dtor"); }

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

typedef ReferenceCountedObjectPtr<Oizo> OizoPtr;

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
#pragma mark -

typedef struct _jojo {

public :
    _jojo() : mOizo (new Oizo()), mLock() { 
        mOizo->setProperty (JojoIdentifier::One, "Carotte");
        mOizo->setProperty (JojoIdentifier::Two, "Olive");
    }

public:
    t_object        ob;
    ulong           mError;
    OizoPtr         mOizo;
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
    const ScopedLock myLock (x->mLock); 
    
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
    test.append (x->mOizo.getObject());
    
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
