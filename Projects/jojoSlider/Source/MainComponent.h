/*
  ==============================================================================

  This is an automatically generated GUI class created by the Introjucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Introjucer version: 3.1.0

  ------------------------------------------------------------------------------

  The Introjucer is part of the JUCE library - "Jules' Utility Class Extensions"
  Copyright 2004-13 by Raw Material Software Ltd.

  ==============================================================================
*/

#ifndef __JUCE_HEADER_9002020A4DD09B20__
#define __JUCE_HEADER_9002020A4DD09B20__

//[Headers]     -- You can add your own extra header files here --

#include "JuceHeader.h"

/* Store a raw t_jojo pointer into a base class to inherit from (instead of juce::Component). */

class JojoComponent : public juce::Component
{
protected:
    JojoComponent (void *o) : jojoPtr (o) { }

protected:
    void *jojoPtr;
};

//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Introjucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class MainComponent  : public JojoComponent,
                       public SliderListener
{
public:
    //==============================================================================
    MainComponent (void *o);
    ~MainComponent();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.

    class MainLookAndFeel : public LookAndFeel_V3
    {
    public:
        MainLookAndFeel()
        {
            setColour (Slider::rotarySliderFillColourId, Colours::orange);
            setColour (Slider::rotarySliderOutlineColourId, Colours::black);
        }
    };

    //[/UserMethods]

    void paint (Graphics& g);
    void resized();
    void sliderValueChanged (Slider* sliderThatWasMoved);



private:
    //[UserVariables]   -- You can add your own custom variables in this section.

    MainLookAndFeel mainLookAndFeel;

    //[/UserVariables]

    //==============================================================================
    ScopedPointer<Slider> slider;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

#endif   // __JUCE_HEADER_9002020A4DD09B20__