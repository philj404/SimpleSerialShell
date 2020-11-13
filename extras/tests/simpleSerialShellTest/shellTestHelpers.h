// shellTestHelpers.h
//
//#include <Streaming.h>
#include <Arduino.h>
#include "simpleFIFO.h"

// SimMonitor simulates a serial terminal
class SimMonitor: public Stream {

    public:
        SimMonitor(void);
        void init(void);
        String getline(void); // get the line sent to display
        int getOutput(void);  // get display output char, or -1 if none

        size_t pressKeys(const char * s);  // send a line
        size_t pressKey(char c);  // simulate a keypress

        // stream emulation
        virtual size_t write(uint8_t);
        virtual int available();
        virtual int read();
        virtual int peek();
        virtual void flush(); // esp32 needs an implementation

    private:
        static const int BUFSIZE = 80;
        SimpleFIFO<char, BUFSIZE> keyboardBuffer;
        SimpleFIFO<char, BUFSIZE> displayBuffer;
};

void addTestCommands(void);
