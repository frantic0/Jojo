
/* 
    { LICENSE }
*/

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------

/* PropertiesFile experiments. */

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------

/* Note: due to the message manager loop, thread-safety needs extra care. */

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

class Oizo : public ChangeListener {

public:
    explicit Oizo()    { cpost ("Oizo ctor\n"); }
    ~Oizo()            { cpost ("Oizo dtor\n"); }
    
public:
    void changeListenerCallback (ChangeBroadcaster*) { post ("Something have changed!"); }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Oizo)
};

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
#pragma mark -

typedef struct _jojo {

public :
    _jojo() : mOizo (new Oizo()), mProperties (nullptr) { 
    //
    /* File is next to the bundle for convenience only. */
    
    File folder (File::getSpecialLocation (File::currentApplicationFile));
    File settings (folder.getSiblingFile ("jojoProperties.txt"));
    mProperties = new PropertiesFile (settings, PropertiesFile::Options());
    mProperties->addChangeListener (mOizo);
    //
    }
    
    ~_jojo() { mProperties->removeChangeListener (mOizo); mProperties->saveIfNeeded(); }
    
public:
    t_object                      ob;
    ulong                         mError;
    ScopedPointer<Oizo>           mOizo;          // --
    ScopedPointer<PropertiesFile> mProperties;    /* Consider to use ApplicationProperties in your code. */
    
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
    
    class_addmethod (c, (method)jojo_bang,       "bang", 0);
    class_addmethod (c, (method)jojo_anything,   "anything", A_GIMME, 0);
    
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
    /* To avoid locking mess manage everything in the main thread. */
    
    if (!systhread_ismainthread()) { error ("Always in the main thread!"); } 
    else {
    //
    // const ScopedLock lock (x->mProperties->getLock());
    post ("Keys: %s", x->mProperties->getAllProperties().getAllKeys().joinIntoString (" / ").toRawUTF8());
    post ("Values: %s", x->mProperties->getAllProperties().getAllValues().joinIntoString (" / ").toRawUTF8());
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
            x->mProperties->setValue (s->s_name, atom_getsym (argv)->s_name); 
        } else {
            x->mProperties->setValue (s->s_name, atom_getfloat (argv)); 
        }
    } 
    //
    }
}

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
