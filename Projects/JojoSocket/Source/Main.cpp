
/* 
    Copyright (c) 2015, Nicolas Danet, < nicolasdanet@free.fr >.

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

/* To get raw binary in Max/MSP the [sadam.udpReceiver] object is needed. */

/* < http://www.sadam.hu/software > */

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------

/* Note that native [udpreceive] object need OSC packets. */

/* http://www.juce.com/forum/topic/sending-messages-juce-maxmsp */
/* https://github.com/danlin/danlin_modules/tree/master/modules */

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------

#include "../JuceLibraryCode/JuceHeader.h"

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
#pragma mark -

class SocketThread : public Thread {

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
#pragma mark -

public:
    SocketThread() : Thread ("Socket")
    {
    }
    
    ~SocketThread()
    {
        Logger::writeToLog ("Shutdown");
    }

public:
    void run() override
    {
        const String localhost ("127.0.0.1");
        const int port = 7400;
        
        String txt ("Le sentier longeait la falaise.");
        const MemoryBlock mb (txt.toRawUTF8(), txt.getNumBytesAsUTF8() + 1);
    
        while (!threadShouldExit()) {
        //
        int err = udp_.waitUntilReady (false, 20);
        
        switch (err) {
            case 0  : Logger::writeToLog ("Timeout"); break;
            case 1  : udp_.write (localhost, port, mb.getData(), mb.getSize()); break;
            default : Logger::writeToLog ("Error"); break;
        }

        sleep (100);
        //
        }
    }

private:
    DatagramSocket udp_;
    
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SocketThread)
};

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
#pragma mark -

class JojoSocketApplication : public JUCEApplication {

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
#pragma mark -

public:
    JojoSocketApplication() 
    {
    }

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
#pragma mark -

public:
    const String getApplicationName() override
    { 
        return ProjectInfo::projectName; 
    }
    const String getApplicationVersion() override
    { 
        return ProjectInfo::versionString; 
    }
    
    bool moreThanOneInstanceAllowed() override 
    { 
        return false; 
    }

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
#pragma mark -

public:
    void initialise (const String& commandLine) override
    {
        socket_ = new SocketThread();
        jassert (socket_ != nullptr);
        socket_->startThread();
    }

    void shutdown() override
    {
        socket_->stopThread (-1);
        socket_ = nullptr;
    }

    void systemRequestedQuit() override
    {
        quit();
    }

    void anotherInstanceStarted (const String& commandLine) override
    {
    }

private:
    ScopedPointer < SocketThread > socket_;
};

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------

START_JUCE_APPLICATION (JojoSocketApplication)

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------
