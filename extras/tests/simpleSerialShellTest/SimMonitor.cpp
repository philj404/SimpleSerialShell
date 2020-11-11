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
    readBuffer.flush();
    writeBuffer.flush();
}

////////////////////////////////////////////////////////////////////////////////
String SimMonitor::getline(void) {
    String theLine;
    theLine.reserve(BUFSIZE);
    while (writeBuffer.count() > 0) {
        theLine += writeBuffer.dequeue();
    }
    return theLine;
}

////////////////////////////////////////////////////////////////////////////////
// get a character sent to the display
int SimMonitor::getOutput(void) {
    int theKey = -1;
    if (writeBuffer.count() > 0) {
        theKey = writeBuffer.dequeue();
    }
    return theKey;
}

////////////////////////////////////////////////////////////////////////////////
// simulate a keypress
size_t SimMonitor::pressKey(char c) {
    return readBuffer.enqueue(c);
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// stream interface
size_t SimMonitor::write(uint8_t aByte) // write to "display"
{
    // carriage return should reset line?
    return writeBuffer.enqueue(aByte);
}

int SimMonitor::available()     // any keypresses?
{
    return (readBuffer.count() > 0);
}

int SimMonitor::read()          // read keyboard input
{
    return readBuffer.dequeue();
}

int SimMonitor::peek()
{
    return readBuffer.peek();
}

void SimMonitor::flush()
{
    init();
}
