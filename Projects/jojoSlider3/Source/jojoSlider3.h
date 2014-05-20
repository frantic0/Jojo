
/*
    { LICENCE }
*/

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------

#ifndef JOJO_SLIDER_3_H
#define JOJO_SLIDER_3_H

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

#include "MainComponent.h"

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
#pragma mark -

class MainWindow : public DocumentWindow {

/* Publicly manage pointer to MainComponent for convenience. */

public:
    MainWindow (void *o) : DocumentWindow ("Jojo", Colours::lightgrey, DocumentWindow::allButtons),
                          mainComponent (new MainComponent (o)) 
    {
        setContentNonOwned (mainComponent, true);
        setSize (400, 400);
        centreWithSize (getWidth(), getHeight());
        setResizable (true, true);
        setUsingNativeTitleBar (true);
    }
    
    void closeButtonPressed() { setVisible (false); }

public:
    ScopedPointer<MainComponent> mainComponent;     /* Private (or delete policy) should be better! */
    
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainWindow)
};

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
#pragma mark -

typedef struct _jojo {

public :
    _jojo() : mValue(), mWindow (new MainWindow (this)) { }

public:
    t_object ob;
    ulong mError;
    Atomic <double> mValue;                         /* Atomic for thread safety. */
    ScopedPointer <MainWindow> mWindow;
    void *mOutlet;
    
    } t_jojo;

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
#endif // JOJO_SLIDER_3_H
