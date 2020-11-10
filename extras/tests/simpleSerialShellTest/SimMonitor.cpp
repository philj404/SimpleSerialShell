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
    while(writeBuffer.count() > 0) {
        theLine += writeBuffer.dequeue();
    }
    return theLine;
}

////////////////////////////////////////////////////////////////////////////////
size_t SimMonitor::pressKey(char c) {
    return readBuffer.enqueue(c);
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// stream interface
size_t SimMonitor::write(uint8_t aByte) {
    // carriage return should reset line?
    return writeBuffer.enqueue(aByte);
}

int SimMonitor::available() {
    return (readBuffer.count() > 0);
}

int SimMonitor::read() {
    return readBuffer.dequeue();
}

int SimMonitor::peek() {
    return readBuffer.peek();
}

void SimMonitor::flush() {
    init();
}
