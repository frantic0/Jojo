
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

/* Slave process. */

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------

#include "../JuceLibraryCode/JuceHeader.h"

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------

class Slave : public ChildProcessSlave, private DeletedAtShutdown {

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
#pragma mark -

public:
    Slave() 
    { 
    }
    
    ~Slave() 
    { 
    }

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
#pragma mark -

public:
    void handleMessageFromMaster (const MemoryBlock& mb) {
    //
    (void)mb;
    String myText ("- I am fine, thank you. And you?");
    const MemoryBlock msg (myText.toRawUTF8(), myText.getNumBytesAsUTF8() + 1);
    sendMessageToMaster (msg);
    //
    }
    
    void handleConnectionMade()    { }
    void handleConnectionLost()    { JUCEApplication::quit(); }
};
    
// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
#pragma mark -

class jojoSlaveApplication : public JUCEApplication {

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
#pragma mark -

public:
    jojoSlaveApplication() { }

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
#pragma mark -

public:
    const String getApplicationName()      { return ProjectInfo::projectName; }
    const String getApplicationVersion()   { return ProjectInfo::versionString; }
    bool moreThanOneInstanceAllowed()      { return true; }

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
#pragma mark -

public:
    void initialise (const String& commandLine) {
    //
    ScopedPointer < Slave > slave (new Slave());

    if (slave->initialiseFromCommandLine (commandLine, "jojoUID")) { slave.release(); }
    else {
        quit();
    }
    //
    }

    void shutdown() { }
    void systemRequestedQuit() { quit(); }
    void anotherInstanceStarted (const String& commandLine) { }
};

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------

START_JUCE_APPLICATION (jojoSlaveApplication)

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
