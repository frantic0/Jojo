
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

/* What time is it? */

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

#include "ext_atomic.h"

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------

#include "JuceHeader.h"

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------

/* Implement a Waster class? */

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
#pragma mark -

typedef struct _waste {
    long step_;
    long index_;
    } t_waste;

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------

static t_int32_atomic counter;

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
#pragma mark -

#define FLOP_SIZE_WASTE 65521L

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------

static long values[FLOP_SIZE_WASTE];

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
#pragma mark -

void waste_constructor  (void);

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
#pragma mark -

#if defined ( __clang__ ) || defined ( __GNUC__ )

void waste_ctor (void)  __attribute__ ((constructor));
void waste_ctor (void)  { waste_constructor(); }

#endif

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------

void waste_constructor (void)
{
    long i;
    unsigned long long seed = time (NULL) & 0xffULL;
    
    for (i = 0; i < FLOP_SIZE_WASTE; i++) {
        values[i] = ((seed = ((seed * 0x5deece66dULL + 0xbULL) & 0xffffffffffffULL)) >> 16);
    }
}

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
#pragma mark -

void waste_init (t_waste *x)
{
    long k = ATOMIC_INCREMENT (&counter);
    
    x->step_ = (k % (FLOP_SIZE_WASTE - 1)) + 1;
    x->index_ = 0;
}

void waste_time (t_waste *x)
{
    long i;
    
    do {
    //
    i = values[x->index_];
    x->index_ += x->step_;
    if (x->index_ >= FLOP_SIZE_WASTE) {
        x->index_ -= FLOP_SIZE_WASTE;
    }
    //
    } while (i & 7);
}

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
#pragma mark -

typedef struct _jojo {

public:
    _jojo() : time_ (Time::getCurrentTime()) { }

public:
    t_object ob_;
    ulong error_;
    Time time_;
    
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

void *jojo_new          (t_symbol *s, long argc, t_atom *argv);
void jojo_free          (t_jojo *x);
void jojo_bang          (t_jojo *x);
void jojo_benchmark     (t_jojo *x);
void jojo_doSomething   (void);

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
#pragma mark -

static t_class *jojo_class;

JOJO_EXPORT int main (void)
{   
    t_class *c = NULL;
    
    c = class_new ("jojoTime", (method)jojo_new, (method)jojo_free, sizeof (t_jojo), NULL, A_GIMME, 0);
    
    class_addmethod (c, (method)jojo_bang,      "bang",         0);
    class_addmethod (c, (method)jojo_benchmark, "benchmark",    0);
    
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
    RelativeTime elapsedTime (Time::getCurrentTime() - x->time_);
    
    post ("Origin / %s", x->time_.toString (true, true, true, true).toRawUTF8());
    post ("Elapsed / %s", elapsedTime.getDescription().toRawUTF8());
}

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
#pragma mark -

void jojo_benchmark (t_jojo *x) 
{
    File folder (File::getSpecialLocation (File::currentApplicationFile).getParentDirectory());
    
    /* Return "jojoTime.txt" "jojoTime2.txt" "jojoTime3.txt"... */
    
    PerformanceCounter benchmark ("Jojo", 100, folder.getNonexistentChildFile ("jojoTime", ".txt", false));

    for (int i = 0; i < 1000; ++i) {
    //
    benchmark.start();
    jojo_doSomething();
    benchmark.stop();
    //
    }
}

void jojo_doSomething (void)
{
    t_waste waste;
    waste_init (&waste);
    
    for (int i = 0; i < 100; ++i) { waste_time (&waste); }       /* Waste only few CPU cycles. */
}

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
