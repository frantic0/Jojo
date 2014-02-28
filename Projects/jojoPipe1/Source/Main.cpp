
/*
 *  Copyright (c) 2014, Nicolas Danet, 
 *  "nicolas.danet@free.fr".
 *
 *  Permission is hereby granted, free of charge, to any person obtaining
 *  a copy of this software and associated documentation files (the
 *  "Software"), to deal in the Software without restriction, including
 *  without limitation the rights to use, copy, modify, merge, publish,
 *  distribute, sublicense, and/or sell copies of the Software, and to
 *  permit persons to whom the Software is furnished to do so, subject to
 *  the following conditions:
 *
 *  The above copyright notice and this permission notice shall be
 *  included in all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 *  LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 *  OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 *  WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------

/* Test / Don't use it. */

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

void intHandler(int dummy = 0) { running = false; }     /* Stop me with CTRL-C please! */

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------

int main(int argc, char* argv[])
{
    signal(SIGINT, intHandler);
    
    NamedPipe pipe;
    pipe.createNewPipe("jojoLapin1234");
    
    String myText("HelloWorld!");
    const MemoryBlock msg(myText.toRawUTF8( ), myText.getNumBytesAsUTF8( ) + 1);
    
    while(running) { 
    //
    DBG("Pipe1"); 
    int result = pipe.write(msg.getData( ), msg.getSize( ), -1);    /* Caution: block until jojoPipe2. */
    DBG(result);
    Thread::sleep(1000);
    //
    }
    
    return 0;
}

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------

