
/* 
    { LICENSE }
*/

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------

/* Public-key encryption. */

/* < https://en.wikipedia.org/wiki/Public-key_cryptography > */

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
    _jojo() : mPublic(), mPrivate(), mLock() { RSAKey::createKeyPair (mPublic, mPrivate, 128); }
 
public:
    t_object        ob;
    ulong           mError;
    RSAKey          mPublic;
    RSAKey          mPrivate;
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
    
    c = class_new ("jojoRSA", (method)jojo_new, (method)jojo_free, sizeof (t_jojo), NULL, A_GIMME, 0);
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

/* < http://www.cs.utexas.edu/~mitra/honors/soln.html > */

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------

void jojo_bang (t_jojo *x)
{
    const ScopedLock myLock (x->mLock); 
    
    post ("Public / %s", x->mPublic.toString().toRawUTF8());
    post ("Private / %s", x->mPrivate.toString().toRawUTF8());
    
    String myText (CharPointer_UTF8 ("P\xc3\xa9p\xc3\xa9 p\xc3\xa8te en ao\xc3\xbbt!"));
    
    post ("%s", myText.toRawUTF8());
    
    const juce::MemoryBlock blockBegin (myText.toRawUTF8(), myText.getNumBytesAsUTF8() + 1);

    BigInteger bitArray;
    bitArray.loadFromMemoryBlock (blockBegin);
    x->mPublic.applyToValue (bitArray);             /* Encrypt with the public key. */

    post ("%s", bitArray.toString (16).toRawUTF8());
    
    x->mPrivate.applyToValue (bitArray);            /* Then decrypt with the private key. */
    
    const juce::MemoryBlock blockEnd (bitArray.toMemoryBlock());
    post ("%s", blockEnd.toString().toRawUTF8());
}

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
