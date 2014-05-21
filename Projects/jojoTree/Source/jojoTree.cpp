
/* 
    { LICENSE }
*/

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------

/* ValueTree and UndoManager basics. */

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

namespace JojoId
{
    #define JOJO_ID(name)   const Identifier name (#name)
    
    JOJO_ID (JojoTree);
    
    #undef JOJO_ID
}

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------

struct _jojo;

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
#pragma mark -

/* ValueTree callbacks are synchronous. */

class Oizo : public ValueTree::Listener, private Timer {                /* Timer for convenience only. */

public:
    explicit Oizo (_jojo* o) : owner_ (o) { startTimer (500); }

public:
    void valueTreePropertyChanged (ValueTree& tree, const Identifier& property) { 
    //
    post ("Changed / %s", property.toString().toRawUTF8());
    //
    }
    
    void valueTreeChildAdded (ValueTree&, ValueTree&)    { }
    void valueTreeChildRemoved (ValueTree&, ValueTree&)  { }
    void valueTreeChildOrderChanged (ValueTree&)         { }
    void valueTreeParentChanged  (ValueTree&)            { }
    void valueTreeRedirected (ValueTree&)                { }

private:
    struct _jojo* owner_; 
    
    void timerCallback();
    
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Oizo)
};

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
#pragma mark -

typedef struct _jojo {

public:
    _jojo() : undo_ (new UndoManager()), oizo_ (new Oizo (this)), tree_ (new ValueTree (JojoId::JojoTree)) { 
    //
    tree_->addListener (oizo_);
    //
    }
    
    ~_jojo() { tree_->removeListener (oizo_); }     /* Currently not necessary, but for future! */

public:
    t_object ob_;
    ulong error_;
    ScopedPointer <UndoManager> undo_;
    ScopedPointer <Oizo> oizo_;
    ScopedPointer <ValueTree> tree_;
                     
    } t_jojo;

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
#pragma mark -

void Oizo::timerCallback() 
{ 
    owner_->undo_->beginNewTransaction(); 
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
void jojo_undo      (t_jojo *x);
void jojo_redo      (t_jojo *x);
void jojo_anything  (t_jojo *x, t_symbol *s, long argc, t_atom *argv);

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
#pragma mark -

static t_class *jojo_class;

JOJO_EXPORT int main (void)
{   
    t_class *c = NULL;
    
    c = class_new ("jojoTree", (method)jojo_new, (method)jojo_free, sizeof (t_jojo), NULL, A_GIMME, 0);
    
    class_addmethod (c, (method)jojo_bang,      "bang",     0);
    class_addmethod (c, (method)jojo_undo,      "undo",     0);
    class_addmethod (c, (method)jojo_redo,      "redo",     0);
    class_addmethod (c, (method)jojo_anything,  "anything", A_GIMME, 0);
    
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

/* Best to keep everything in the main thread. */

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------

void jojo_bang (t_jojo *x) 
{
    if (systhread_ismainthread()) {        
        post ("%s", x->tree_->toXmlString().toRawUTF8());
    }
}

void jojo_undo (t_jojo *x) 
{
    if (systhread_ismainthread()) {        
        post ("Undo / %ld", x->undo_->undo());
    }
}

void jojo_redo (t_jojo *x) 
{
    if (systhread_ismainthread()) {
        post ("Redo / %ld", x->undo_->redo());
    }
}

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------

void jojo_anything (t_jojo *x, t_symbol *s, long argc, t_atom *argv)
{
    if (!systhread_ismainthread()) { error ("Always in the main thread!"); }        
    else if (argc) {
    //
    const String property (s->s_name);
    
    if (Identifier::isValidIdentifier (property)) {
    //
    if (atom_gettype (argv) == A_SYM) { 
        x->tree_->setProperty (property, atom_getsym (argv)->s_name, x->undo_);
    } else {
        x->tree_->setProperty (property, atom_getfloat (argv), x->undo_);
    }
    //
    }
    //
    }
}

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
