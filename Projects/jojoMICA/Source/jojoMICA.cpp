
/* 
    { LICENSE }
*/

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------

/* < https://github.com/burnson/MICA > */

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

#include "Modules/MICA/Ports/en-cpp/mica.h"
#include "Modules/MICA/Ports/en-cpp/mica.cpp"

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------

#include <iostream>

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------

using namespace std;

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
#pragma mark -

typedef struct _jojo {

public:
    _jojo() : lock_() { }

public:
    t_object ob_;
    ulong error_;
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
void jojo_foo   (t_jojo *x);
void jojo_int   (t_jojo *x, long n);

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
#pragma mark -

static t_class *jojo_class;

JOJO_EXPORT int main (void)
{   
    t_class *c = NULL;
    
    c = class_new ("jojoMICA", (method)jojo_new, (method)jojo_free, sizeof (t_jojo), NULL, A_GIMME, 0);
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
    ScopedLock sl (x->lock_);

    cout << mica::Intervals::transpose (mica::C4, mica::PerfectOctave, mica::Above) << endl;
    cout << mica::Intervals::transpose (mica::G4, mica::PerfectOctave, mica::Above) << endl;
    cout << mica::Intervals::transpose (mica::E4, mica::MajorSeventh,  mica::Above) << endl;
    cout << mica::Intervals::transpose (mica::C4, mica::PerfectOctave, mica::Below) << endl;
    cout << mica::Intervals::transpose (mica::G4, mica::PerfectOctave, mica::Below) << endl;
    cout << mica::Intervals::transpose (mica::E4, mica::MajorSeventh,  mica::Below) << endl;
    
    // C5
    // G5
    // D4 sharp
    // C3
    // G3
    // F4

    mica::Concept c = mica::C;
    
    cout << c << " / " << mica::name (c) << endl;
    cout << c.high << " / " << c.low << endl;
    cout << mica::named ("en:F") << endl;               // --
    
    // C / C
    // 6790648215879694701 / 10123514385212891730
    // F

    cout << map (mica::A, mica::B) << endl;
    cout << map (mica::B, mica::A) << endl;
    cout << map (mica::A, mica::Undefined) << endl;

    // Whole Step
    // Whole Step
    // (undefined)

    cout << (sequence (mica::Letters) ? "Yes" : "No") << endl;
    cout << (cyclic (mica::Letters) ? "Yes" : "No") << endl;
    cout << length (mica::Letters) << endl;
  
    // Yes
    // Yes
    // 7

    for (int i = 0; i < length (mica::Letters); ++i) { cout << item (mica::Letters, i) << endl; }
    for (int i = 0; i < length (mica::Letters); ++i) { cout << item (mica::Letters, mica::C, i) << endl; }
    
    // A B C D E F G
    // C D E F G A B

    cout << index (mica::Letters, mica::C) << endl;
    cout << index (mica::Letters, mica::Undefined) << endl;
    cout << numerator (index (mica::Letters, mica::C, mica::G)) << endl;
    cout << numerator (index (mica::Letters, mica::C)) << endl;

    // (integer)
    // (undefined)
    // 4
    // 2

    mica::Concept r = index (mica::Letters, mica::C);
    
    if (r == mica::Undefined) { cout << "Aborting!" << endl; return; }
    if (mica::undefined (r))  { cout << "Aborting!" << endl; return; }
  
    // 
    
    cout << item (mica::Letters, index (mica::Letters, mica::C)) << endl;
    cout << item (mica::Letters, index (mica::Letters, mica::Undefined)) << endl;
    
    // C
    // (undefined)

    cout << numerator (index (mica::Letters, mica::C, mica::E)) << endl;
    cout << item (mica::Letters, mica::C, 100) << endl;
    cout << numerator (index (mica::Accidentals, mica::Natural, mica::Flat)) << endl;

    // 2
    // E
    // -1

    cout << map (mica::D, mica::Flat) << endl;
    cout << map (mica::D, mica::Flat, mica::integer (4)) << endl;
    cout << map (mica::D, mica::integer (4)) << endl;
  
    // D flat
    // D4 flat
    // D4

    cout << map (mica::DFlat4, mica::DiatonicPitch) << endl;
    cout << map (mica::DFlat4, mica::ChromaticNote) << endl;
    cout << map (mica::DFlat4, mica::Accidental) << endl;
    cout << map (mica::DFlat4, mica::Letter) << endl;
    cout << numerator (map (mica::DFlat4, mica::Octave)) << endl;
    cout << numerator (map (mica::DFlat4, mica::MIDIKeyNumber)) << endl;

    // D4
    // D flat
    // Flat
    // D
    // 4
    // 61

    cout << numerator (map (mica::D4, mica::TrebleClef)) << endl;
    cout << numerator (map (map (mica::DSharp4, mica::DiatonicPitch), mica::BassClef)) << endl;
    cout << map (mica::integer (2), mica::TrebleClef) << endl;
    cout << map (map (mica::integer (2), mica::TrebleClef), mica::Flat) << endl;

    // -5
    // 7
    // D5
    // D5 flat

    cout << map (mica::DFlat, mica::Major) << endl;
    cout << map (mica::DFlat, mica::Dorian) << endl;
    cout << map (mica::DFlatMajor, mica::KeySignature) << endl;
    cout << map (mica::DFlatDorian, mica::KeySignature) << endl;
    cout << map (mica::ThreeFlats, mica::Minor) << endl;
    cout << map (mica::DFlatMajor, mica::Mode) << endl;
    cout << map (mica::DFlatDorian, mica::Mode) << endl;
  
    // D-flat Major
    // D-flat Dorian
    // Five Flats
    // Seven Flats
    // C Minor
    // Major
    // Dorian

    {
        mica::Concept SharpPositions = map (mica::TrebleClef, mica::Sharp);
        cout << SharpPositions << endl;
        for (int i = 0; i < length (SharpPositions); ++i) { 
            cout << numerator (item (SharpPositions, i)) << endl;
        }
    }
  
    // Treble Clef Sharps
    // 4 1 5 2 -1 3 0

    cout << map (mica::Fourth, mica::Perfect) << endl;
    cout << map (mica::PerfectFourth, mica::Distance) << endl;
    cout << map (mica::PerfectFourth, mica::Quality) << endl;

    // Perfect Fourth
    // Fourth
    // Perfect

    cout << mica::Intervals::interval (mica::D5, mica::B3) << endl;
    cout << mica::Intervals::quality  (mica::D5, mica::B3) << endl;
    cout << mica::Intervals::distance (mica::D5, mica::B3) << endl;
    
    // Minor Third
    // Minor
    // Third

    cout << numerator (mica::Intervals::octaves (mica::D5, mica::B3)) << endl;
    cout << mica::Intervals::direction (mica::D5, mica::B3) << endl;
    cout << map (mica::TwoSharps, mica::MinorSecond, mica::Above) << endl;
    
    // 1
    // Below
    // Three Flats
}

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
