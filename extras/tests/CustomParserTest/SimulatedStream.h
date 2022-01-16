#ifndef _TestStream_h
#define _TestStream_h

#include <Arduino.h>
#include "simpleFIFO.h"

/**
 * @brief A simple implementation fo the Stream interface.  Useful for 
 * creating mock Serial objects for unit testing.
 * 
 * A header-only class for ease of re-use across tests cases.
 */
template<int MAX_BUFSIZE> 
class SimulatedStream : public Stream {
public:

    SimulatedStream() { }
    virtual ~SimulatedStream() { }

    void init() {
        _outputBuffer.flush();
        _inputBuffer.flush();
    }

    String getline() {
        return getOutput();
    }

    /**
     * @brief Returns the accumulated output and then flushes.
     */
    String getOutput() {
        String result;
        result.reserve(MAX_BUFSIZE);
        while (_outputBuffer.count() > 0) {
            result.concat(_outputBuffer.dequeue());
        }
        return result;
    }

    /**
     * @brief Simulate the entry of multiple inbound characters.
     * 
     * @param keys 
     */
    void pressKeys(const char* keys) {
        for (unsigned int i = 0; keys[i]; i++) {
            pressKey(keys[i]);
        }
    }

    /**
     * @brief Simulates an inbound character (ex: a serial keypress)
     * 
     * @param key
     */
    void pressKey(char key) {
        _inputBuffer.enqueue(key);
    }

    // ------------------------------------------------------------------------
    // Implementation of the Stream interface.

    virtual size_t write(uint8_t c) {
        _outputBuffer.enqueue(c);
        return 1;
    }

    virtual int available() {
        return _inputBuffer.count();
    }

    virtual int read() {
        return available() ? _inputBuffer.dequeue() : -1;
    }
    
    virtual int peek() {
        return available() ? _inputBuffer.peek() : -1;
    }

    virtual void flush() {
    }

private:

    SimpleFIFO<char, MAX_BUFSIZE> _inputBuffer;
    SimpleFIFO<char, MAX_BUFSIZE> _outputBuffer;
};

#endif
