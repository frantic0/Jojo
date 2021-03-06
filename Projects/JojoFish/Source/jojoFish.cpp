
/* 
    Copyright (c) 2014, Nicolas Danet, < nicolasdanet@free.fr >.

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

/* BlowFish and MemoryBlock. */

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

static uint8 myKey[] = { 0xFA, 0xDA, 0xFA, 0xDA };     /* Never hard code password (don't do that)! */

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
#pragma mark -

typedef struct _jojo {

public:
    _jojo() : fish_ (myKey, numElementsInArray (myKey)), lock_() { }

public:
    t_object ob_;
    ulong error_;
    BlowFish fish_;
    CriticalSection lock_;
    
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

void jojo_write (t_jojo *x, const File& aFile);
void jojo_read  (t_jojo *x, const File& aFile);

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
#pragma mark -

static t_class *jojo_class;

JOJO_EXPORT int main (void)
{   
    t_class *c = NULL;
    
    c = class_new ("jojoFish", (method)jojo_new, (method)jojo_free, sizeof (t_jojo), NULL, A_GIMME, 0);
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
    
    File myFile ((File::getSpecialLocation (File::currentApplicationFile)).getSiblingFile ("jojoFish.txt"));
    
    jojo_write (x, myFile);
    jojo_read (x, myFile);
}

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
#pragma mark -

/* Example padded with zeros with ECB to chain the blocks. */

/* < https://en.wikipedia.org/wiki/Block_cipher_mode_of_operation > */

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------

void jojo_write (t_jojo *x, const File& aFile)
{
    String myText (CharPointer_UTF8 ("P\xc3\xa9p\xc3\xa9 p\xc3\xa8te en ao\xc3\xbbt!"));

    myText  << newLine << "###" << newLine
            << "Stately, plump Buck Mulligan came from the stairhead, " << newLine
            << "bearing a bowl of lather on which a mirror and a razor lay crossed." << newLine;
    
    const size_t textSize = myText.getNumBytesAsUTF8() + 1;     /* UTF_8 size != string length. */
    const size_t blockSize = (textSize + 7) & ~7;               /* Round up to the next multiple of 8. */

    /* Pad with zeros (handy for a null terminated string). */
    
    juce::MemoryBlock block (blockSize, true);
    block.copyFrom (myText.toRawUTF8(), 0, textSize);         
    
    for (size_t i = 0; i < blockSize; i += 8) {
    //
    x->fish_.encrypt (reinterpret_cast < uint32& > (block[i]), reinterpret_cast < uint32& > (block[i + 4]));
    //
    }
    
    aFile.replaceWithData (block.getData(), blockSize);
}
    
void jojo_read (t_jojo *x, const File& aFile)
{
    juce::MemoryBlock block;
    
    if (aFile.loadFileAsData (block)) {
    //
    const size_t blockSize = block.getSize();
    
    for (size_t i = 0; i < blockSize; i += 8) {
    //
    x->fish_.decrypt (reinterpret_cast < uint32& > (block[i]), reinterpret_cast < uint32& > (block[i + 4]));
    //
    }
    
    StringArray myText (StringArray::fromLines (block.toString().toRawUTF8()));
    
    for (int i = 0; i < myText.size(); ++i) {
        post ("%s", myText.getReference (i).toRawUTF8());
    }
    //
    }
}
    
// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
