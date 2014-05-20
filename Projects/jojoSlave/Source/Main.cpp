
/* 
    { LICENSE }
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

public:
    explicit Slave()    { }
    ~Slave()            { }

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

class jojoSlaveApplication : public JUCEApplication {

public:
    jojoSlaveApplication() { }

    const String getApplicationName()      { return ProjectInfo::projectName; }
    const String getApplicationVersion()   { return ProjectInfo::versionString; }
    bool moreThanOneInstanceAllowed()      { return true; }

    void initialise (const String& commandLine) {
    //
    ScopedPointer <Slave> slave (new Slave());

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
