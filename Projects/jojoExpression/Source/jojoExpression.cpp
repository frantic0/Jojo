
/* 
    { LICENSE }
*/

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------

/* Calculator (unscientific). */

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

/* < http://sourcemaking.com/design_patterns/visitor/cpp/2 > */

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------

class aScope : public Expression::Scope {

public:
    String getScopeUID() const { return String ("A scope!"); }
};

class aVisitor : public Expression::Scope::Visitor {
    
public:
    void visit (const Expression::Scope& scope) { post ("%s", scope.getScopeUID().toRawUTF8()); }
};

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
#pragma mark -

class Oizo : public Expression::Scope
{

public:    
    Expression getSymbolValue (const String& symbol) const {
        if (symbol == "two")        { return Expression (2); }
        else if (symbol == "five")  { return Expression (5); }
        else if (symbol == "x")     { return Expression ("two + five"); }
        
        return Expression::Scope::getSymbolValue (symbol);   /* Call base class function. */
    }
    
    double evaluateFunction (const String& functionName, const double* parameters, int numParams) const {
        if ((numParams > 0) && (functionName == "half")) { return (parameters[0] / 2.); }
        return Expression::Scope::evaluateFunction (functionName, parameters, numParams);
    }
    
    void visitRelativeScope (const String& scopeName, Visitor& visitor) const {
        if (scopeName == "Foo") { visitor.visit (aScope()); return; }
        Expression::Scope::visitRelativeScope (scopeName, visitor);
    }
};

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
#pragma mark -

typedef struct _jojo {

public :
    _jojo() : mScope(), mLock() { }

public:
    t_object        ob;
    ulong           mError;
    Oizo            mScope;
    CriticalSection mLock;                  /* I guess that is not thread-safe, isn't it? */
    
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
void jojo_visitor   (t_jojo *x);

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
#pragma mark -

static t_class *jojo_class;

JOJO_EXPORT int main (void)
{   
    t_class *c = NULL;
    
    c = class_new ("jojoExpression", (method)jojo_new, (method)jojo_free, sizeof (t_jojo), NULL, A_GIMME, 0);
    class_addmethod (c, (method)jojo_bang,       "bang",     0);
    class_addmethod (c, (method)jojo_visitor,    "visitor",  0);
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
    
    Expression a ("(two + 2) * five");
    Expression b ("x * 2");
    
    post ("%s = %f", a.toString().toRawUTF8(), a.evaluate (x->mScope));  
    post ("%s = %f", b.toString().toRawUTF8(), b.evaluate (x->mScope));   
    
    Expression c = a - b;
    
    post ("%s = %f", c.toString().toRawUTF8(), c.evaluate (x->mScope));
    
    Expression d ("half(five)");
    
    post ("%s = %f", d.toString().toRawUTF8(), d.evaluate (x->mScope));
}

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
#pragma mark -

/* Actually, I don't know what's the point of visitRelativeScope method! */

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------

void jojo_visitor (t_jojo *x)
{
    aVisitor visitor;
    x->mScope.visitRelativeScope ("Foo", visitor);
}

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
