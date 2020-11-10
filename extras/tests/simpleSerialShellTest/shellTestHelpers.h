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
        String getline(void); // get the line written to stream
        void sendline(const String & s);

        // stream emulation
        virtual size_t write(uint8_t);
        virtual int available();
        virtual int read();
        virtual int peek();
        virtual void flush(); // esp32 needs an implementation

    private:
        static const int BUFSIZE = 80;
        SimpleFIFO<char, BUFSIZE> readBuffer;
        SimpleFIFO<char, BUFSIZE> writeBuffer;
};

void addTestCommands(void);
