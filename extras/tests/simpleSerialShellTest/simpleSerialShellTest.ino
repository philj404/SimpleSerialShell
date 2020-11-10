//
// simpleSerialShellTest.ino
//
// Confirm serial shell behaves as expected.

// UnixHostDuino emulation needs this include
// (it's not picked up "for free" by Arduino IDE)
//
#include <Arduino.h>

// fake it for UnixHostDuino emulation
#if defined(UNIX_HOST_DUINO)
#  ifndef ARDUINO
#  define ARDUINO 100
#  endif
#endif

//
// also, you need to provide your own forward references

// These tests depend on the Arduino "AUnit" library
#include <AUnit.h>
#include <SimpleSerialShell.h>
//#include <Streaming.h>
#include "shellTestHelpers.h"


SimMonitor terminal;

void prepForTests(void)
{
}

//////////////////////////////////////////////////////////////////////////////
// confirm tasks can be cancelled.
test(test_execute) {
    prepForTests();

    int response = shell.execute("echo hello world");
    assertEqual(response, 0);

    assertEqual(terminal.getline(), "hello world\r\n");

    response = shell.execute("sum 1 2 3");
    assertEqual(response, 6);
};

//////////////////////////////////////////////////////////////////////////////
// ... so which sketch is this?
int showID(int argc = 0, char **argv = NULL)
{
    Serial.println();
    Serial.println(F( "Running " __FILE__ ", Built " __DATE__));
    return 0;
};

//////////////////////////////////////////////////////////////////////////////
void setup() {
    ::delay(1000); // wait for stability on some boards to prevent garbage Serial
    Serial.begin(115200); // ESP8266 default of 74880 not supported on Linux
    while (!Serial); // for the Arduino Leonardo/Micro only
    showID();
    shell.attach(terminal);
    shell.addCommand(F("id?"), showID);

    addTestCommands();
}

//////////////////////////////////////////////////////////////////////////////
void loop() {
    // Should get:
    // TestRunner summary:
    //    <n> passed, <n> failed, <n> skipped, <n> timed out, out of <n> test(s).
    aunit::TestRunner::run();
}
