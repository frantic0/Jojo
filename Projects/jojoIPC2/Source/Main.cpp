
/* 
    { LICENSE }
*/

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------

/* IPC Test / Don't use it. */

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------

#include "../JuceLibraryCode/JuceHeader.h"

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------

#include <signal.h>

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------

static bool running = true;

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------

void intHandler (int dummy = 0) { running = false; }     /* Stop me with CTRL-C please! */

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------

class SlaveIPC : public InterprocessConnection {

public : 
    SlaveIPC() : InterprocessConnection (false) { connectToPipe ("jojoLapin1234", -1); }
    ~SlaveIPC() { }

public:
    void connectionMade()  { DBG ("Slave ConnectionMade"); }
    void connectionLost()  { DBG ("Slave ConnectionLost"); }
 
    void messageReceived (const MemoryBlock& mb) { 
    //
    DBG (mb.toString()); 
    String myText ("- I am fine, thank you. And you?");
    const MemoryBlock msg (myText.toRawUTF8(), myText.getNumBytesAsUTF8() + 1);
    sendMessage (msg);
    }
};

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------

int main (int argc, char* argv[])
{
    signal (SIGINT, intHandler);
    
    ScopedPointer<SlaveIPC> myIPC (new SlaveIPC());
    
    while (running) {
    //
    Thread::sleep (1000);
    //
    }
    
    return 0;
}

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------

