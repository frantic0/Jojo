
/* 
    { LICENSE }
*/

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------

/* Simple slider / Timerthread issue / findChildWithID. */

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------

#include "JojoSlider2.h"

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

void *jojo_new  (t_symbol *s, long argc, t_atom *argv);
void jojo_free  (t_jojo *x);
void jojo_bang  (t_jojo *x);
void jojo_float (t_jojo *x, double f);

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------

void jojo_doBang    (t_jojo *x, t_symbol *s, long argc, t_atom *argv);
void jojo_doFloat   (t_jojo *x, t_symbol *s, long argc, t_atom *argv);

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
#pragma mark -

static t_class *jojo_class;

JOJO_EXPORT int main (void)
{   
    t_class *c = NULL;
    
    c = class_new ("jojoSlider2", (method)jojo_new, (method)jojo_free, sizeof (t_jojo), NULL, A_GIMME, 0);
    class_addmethod (c, (method)jojo_bang,   "bang",     0);
    class_addmethod (c, (method)jojo_float,  "float",    A_FLOAT, 0);
    class_register (CLASS_BOX, c);
    jojo_class = c;
    
    JOJO_INITIALIZE
    
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
    
    if (!err) {
        err |= !(x->mOutlet = floatout ((t_object *)x));
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
    defer_low (x, (method)jojo_doBang, NULL, 0, NULL);
}

void jojo_float (t_jojo *x, double f)
{
    t_atom a;

    atom_setfloat (&a, f);
    defer_low (x, (method)jojo_doFloat, NULL, 1, &a);        /* Calls to JUCE always in the main thread. */
    
    outlet_float (x->mOutlet, f);
}

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
#pragma mark -

void jojo_doBang (t_jojo *x, t_symbol *s, long argc, t_atom *argv)
{
    x->mWindow->setVisible (true);
}

/* Fetch the slider in the component tree. */

void jojo_doFloat (t_jojo *x, t_symbol *s, long argc, t_atom *argv)
{
    juce::Slider *q = nullptr;
    juce::Component *p = x->mWindow->findChildWithID ("myComponent"); 
    
    if ((p != nullptr) && ((q = dynamic_cast <juce::Slider *> (p->findChildWithID ("mySlider"))) != nullptr)) {
        if (argc && argv) { q->setValue (atom_getfloat (argv), dontSendNotification); }
    }
}

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
