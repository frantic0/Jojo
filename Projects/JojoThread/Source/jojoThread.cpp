
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

/* Custom thread. */

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------

/* Threads are created suspended, and signaled right after. */
/* All the thread creation machinery is done in the startThread() method. */

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

struct _jojo;

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
#pragma mark -

class JojoThread : public Thread {

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
#pragma mark -

public:
    explicit JojoThread (_jojo *x) : Thread ("Jojo"), owner_ (x) { }
    
public:
    void run();  

private:
    _jojo *owner_;   /* Needed to call t_jojo's functions. */
};

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
#pragma mark -

typedef struct _jojo {

public:
    _jojo() : thread_ (new JojoThread (this)) { thread_->startThread(); }  
    ~_jojo() { thread_->stopThread (-1); }  /* Must be stopped before deletion. */

public:
    t_object ob_;
    ulong error_;
    ScopedPointer < JojoThread > thread_;
    void *clock_; 
    
    } t_jojo;

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
#pragma mark -

void JojoThread::run()
{ 
    static long counter = 0;
    
    while (!threadShouldExit()) { 
    //
    if (++counter % 100) { Thread::sleep (10); }
    else {
        clock_fdelay (owner_->clock_, 0.0);             /* Always use a clock from a custom thread! */
    }
    //
    } 
}

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
void jojo_task  (t_jojo *x);
void jojo_bang  (t_jojo *x);

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
#pragma mark -

static t_class *jojo_class;

JOJO_EXPORT int main (void)
{   
    t_class *c = NULL;
    
    c = class_new ("jojoThread", (method)jojo_new, (method)jojo_free, sizeof (t_jojo), NULL, A_GIMME, 0);
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
    
    err |= !(x->clock_ = clock_new (x, (method)jojo_task));
    
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
    
    if (x->clock_) {
        object_free (x->clock_);
    }
}

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
#pragma mark -

void jojo_task (t_jojo *x)
{
    post ("Le sentier longeait la falaise.");
}

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
#pragma mark -

void jojo_bang (t_jojo *x)
{
    post ("I am the %s thread!", x->thread_->getThreadName().toRawUTF8());
}

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
