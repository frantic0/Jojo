
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

class MasterIPC : public InterprocessConnection {

public : 
    MasterIPC() : InterprocessConnection (false) { createPipe ("jojoLapin1234", -1); }
    ~MasterIPC() { }

public:
    void connectionMade()  { DBG ("Master ConnectionMade"); }
    void connectionLost()  { DBG ("Master ConnectionLost"); }
 
    void messageReceived (const MemoryBlock& mb) { DBG (mb.toString()); }
};
 
// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------

int main (int argc, char* argv[])
{
    signal (SIGINT, intHandler);
    
    ScopedPointer<MasterIPC> myIPC (new MasterIPC());
    
    String myText ("- How are you?");
    const MemoryBlock msg (myText.toRawUTF8(), myText.getNumBytesAsUTF8() + 1);
    
    while (running) { 
    //
    myIPC->sendMessage (msg);     /* Caution: block until jojoIPC2. */
    Thread::sleep (1000);
    //
    }
    
    return 0;
}

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------

