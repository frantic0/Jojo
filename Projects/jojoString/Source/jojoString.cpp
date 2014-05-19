
/* 
    { LICENSE }
*/

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------

/* JUCE::String miscellaneous. */

/* < https://github.com/julianstorer/JUCE/blob/master/modules/juce_core/text/juce_String.cpp#L2134 > */

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

typedef struct _jojo {

public :
    _jojo() : mPair(), mLock() { 
        /* Use a text file to translate strings in the bundle. */
        
        File tr(File::getSpecialLocation(File::currentApplicationFile).getSiblingFile("jojoString.txt"));
        if (tr.existsAsFile()) {
            LocalisedStrings::setCurrentMappings(new LocalisedStrings(tr, false));
        }
    }

public:
    t_object        ob;
    ulong           mError;
    StringPairArray mPair;          /* Not thread-safe. */
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

void *jojo_new      (t_symbol *s, long argc, t_atom *argv);
void jojo_free      (t_jojo *x);
void jojo_bang      (t_jojo *x);
void jojo_anything  (t_jojo *x, t_symbol *s, long argc, t_atom *argv);

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
#pragma mark -

static t_class *jojo_class;

JOJO_EXPORT int main(void)
{   
    t_class *c = NULL;
    
    c = class_new("jojoString", (method)jojo_new, (method)jojo_free, sizeof(t_jojo), NULL, A_GIMME, 0);
    class_addmethod(c, (method)jojo_bang,       "bang", 0);
    class_addmethod(c, (method)jojo_anything,   "anything", A_GIMME, 0);
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
    /* Caution : translation is a linear lookup with a pair of StringArray. */
    
    post("%s", TRANS("The trail skirted the cliff.").toRawUTF8());     /* Should be in french! */
    
    //
    
    String toto(CharPointer_UTF8("P\xc3\xa9p\xc3\xa9 p\xc3\xa8te en ao\xc3\xbbt!"));
    
    post("%s", toto.toRawUTF8());
    post("    Length: %ld", toto.length());
    post("    Bytes: %ld", CharPointer_UTF8::getBytesRequiredFor(toto.getCharPointer()));
    
    //
    
    const ScopedLock myLock(x->mLock); 
    
    post("Keys: %s", x->mPair.getAllKeys().joinIntoString(" / ").toRawUTF8());
    post("Values: %s", x->mPair.getAllValues().joinIntoString(" / ").toRawUTF8());
}

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------

void jojo_anything(t_jojo *x, t_symbol *s, long argc, t_atom *argv)
{
    const ScopedLock myLock(x->mLock); 
    
    if (argc && (atom_gettype(argv) == A_SYM)) {
        x->mPair.set(s->s_name, atom_getsym(argv)->s_name);             /* A StringPairArray exemple. */
    }
}

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
