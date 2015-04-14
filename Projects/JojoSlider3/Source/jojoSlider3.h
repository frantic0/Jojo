
/*
    Copyright (c) 2014, Nicolas Danet, < nicolas.danet@free.fr >.

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
#pragma mark -

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

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
#pragma mark -

/* Publicly manage pointer to MainComponent for convenience. */

public:
    MainWindow (void *o) : DocumentWindow ("Jojo", Colours::lightgrey, DocumentWindow::allButtons),
                           mainComponent_ (new MainComponent (o)) 
    {
        setContentNonOwned (mainComponent_, true);
        setSize (400, 400);
        centreWithSize (getWidth(), getHeight());
        setResizable (true, true);
        setUsingNativeTitleBar (true);
    }

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
#pragma mark -

public:
    void closeButtonPressed() { setVisible (false); }

public:
    ScopedPointer < MainComponent > mainComponent_;     /* Private (or delete policy) should be better! */
    
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainWindow)
};

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
#pragma mark -

typedef struct _jojo {

public:
    _jojo() : value_(), window_ (new MainWindow (this)) { }

public:
    t_object ob_;
    ulong error_;
    Atomic < double > value_;                           /* Atomic for thread safety. */
    ScopedPointer < MainWindow > window_;
    void *outlet_;
    
    } t_jojo;

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
#endif // JOJO_SLIDER_3_H
