
/* 
    { LICENSE }
*/

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------

/* Var games. */

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

class Oizo : public ReferenceCountedObject {

public:
    Oizo()  { post ("Oizo ctor"); }
    ~Oizo() { post ("Oizo dtor"); }
    
    Oizo (const Oizo& o)                { post ("Oizo copy"); }
    Oizo& operator = (const Oizo& o)    { post ("Oizo = "); return *this; }
    void doSomething() const            { post ("Cui-cui"); }
};

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
#pragma mark -

typedef struct _jojo {

public:
    _jojo() { }

public:
    t_object ob_;
    ulong error_;
    
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
    
    c = class_new ("jojoVariant", (method)jojo_new, (method)jojo_free, sizeof (t_jojo), NULL, A_GIMME, 0);
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
    var v (1234);
    post ("Var / %s", v.toString().toRawUTF8());
    
    v = 3.14;
    post ("Var / %s", v.toString().toRawUTF8());
    
    //
    
    v = "Carotte";
    post ("IsArray / %ld", v.isArray());
    
    v.append ("Lapin");
    v.append ("Fusil");
    post ("IsArray / %ld", v.isArray());
    
    post ("Var / %s", v[0].toString().toRawUTF8());
    post ("Var / %s", v[1].toString().toRawUTF8());
    post ("Var / %s", v[2].toString().toRawUTF8());
    
    //
    
    v = var::null;
    
    //
    
    v = var (Array < var >());
    Array < var > * const temp = v.getArray();
    
    temp->add (var ("Toto"));
    temp->add (var ("Jojo"));
    temp->add (var ("1234"));
    temp->add (var::undefined());
    
    for (int i = 0; i < temp->size(); ++i) {
        post ("Var / %s", temp->getUnchecked (i).toString().toRawUTF8());
    }
    
    //
    
    v = var (new Oizo());
    
    var w = v;
    post ("RefCount / %ld", v.getObject()->getReferenceCount());
    
    Oizo* const q = dynamic_cast < Oizo* > (v.getObject());
    if (q != nullptr) { q->doSomething(); }
}

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
