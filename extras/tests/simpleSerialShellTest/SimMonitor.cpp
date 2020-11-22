// SimMonitor.cpp
//
#include <Arduino.h>
//#include <SimpleSerialShell.h>
#include "shellTestHelpers.h"

////////////////////////////////////////////////////////////////////////////////
//
SimMonitor::SimMonitor(void) {
    init();
}

void SimMonitor::init(void) {
    keyboardBuffer.flush();
    displayBuffer.flush();
}

////////////////////////////////////////////////////////////////////////////////
String SimMonitor::getline(void) {
    String theLine;
    theLine.reserve(BUFSIZE);
    while (displayBuffer.count() > 0) {
        theLine += displayBuffer.dequeue();
    }
    return theLine;
}

////////////////////////////////////////////////////////////////////////////////
// get a character sent to the display
int SimMonitor::getOutput(void) {
    int theKey = -1;
    if (displayBuffer.count() > 0) {
        theKey = displayBuffer.dequeue();
    }
    return theKey;
}

////////////////////////////////////////////////////////////////////////////////
// simulate a keypress
size_t SimMonitor::pressKey(char c) {
    return keyboardBuffer.enqueue(c);
}

////////////////////////////////////////////////////////////////////////////////
// simulate a bunch of keypresses
size_t SimMonitor::pressKeys(const char * keys) {
    size_t numSent = 0;

    for (int i = 0; keys[i]; i++) {
        numSent += pressKey(keys[i]);
    }

    return numSent;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// stream interface
size_t SimMonitor::write(uint8_t aByte) // write to "display"
{
    // carriage return should reset line?
    return displayBuffer.enqueue(aByte);
}

int SimMonitor::available()     // any keypresses?
{
    return (keyboardBuffer.count() > 0);
}

int SimMonitor::read()          // read keyboard input
{
    return available() ? keyboardBuffer.dequeue() : -1;
}

int SimMonitor::peek()
{
    return available() ? keyboardBuffer.peek() : -1;
}

void SimMonitor::flush()
{
    init();
}
