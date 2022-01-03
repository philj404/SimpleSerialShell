// HelpTest.ino
//
#include <Arduino.h>

// fake it for UnixHostDuino emulation
#if defined(UNIX_HOST_DUINO)
#  ifndef ARDUINO
#  define ARDUINO 100
#  endif
#endif

// These tests depend on the Arduino "AUnit" library
#include <AUnit.h>
#include "SimMonitor.h"
#include <SimpleSerialShell.h>

// Some platforms ouput line endings differently.
#define NEW_LINE "\r\n"
#define HELP_PREAMBLE "Commands available are:"
#define TWO_SPACE "  "

// A mock of the Arduino Serial stream
static SimMonitor terminal;

void prepForTests(void)
{
    terminal.init();
    shell.resetBuffer();
}

//////////////////////////////////////////////////////////////////////////////
// test fixture to ensure clean initial and final conditions
//
class ShellTest: public aunit::TestOnce {
    protected:
        void setup() override {
            TestOnce::setup();
            prepForTests();
        }

        void teardown() override {
            prepForTests();
            TestOnce::teardown();
        }
};

int echo(int, char **) 
{
    return 0;
}

int rangeCommand(int, char **) 
{
    return 0;
}

//////////////////////////////////////////////////////////////////////////////
// The goal of this test is to validate that the help messages
// return as expected.
testF(ShellTest, helpTest) 
{
    const char* testCommand = "help\r";
    terminal.pressKeys(testCommand);
    assertTrue(shell.executeIfInput());

    // The begining part of the response is the echo of the user's input.
    assertEqual(terminal.getline(),  
        ("help" NEW_LINE 
        HELP_PREAMBLE NEW_LINE 
        TWO_SPACE "echo" NEW_LINE 
        TWO_SPACE "help" NEW_LINE 
        TWO_SPACE "range <lower> <upper>" NEW_LINE));
}

//////////////////////////////////////////////////////////////////////////////
void setup() {
    ::delay(1000); // wait for stability on some boards to prevent garbage Serial
    Serial.begin(115200); // ESP8266 default of 74880 not supported on Linux
    while (!Serial); // for the Arduino Leonardo/Micro only

    shell.addCommand(F("echo"), echo);
    // Register a command with argument documentation (just a string)
    shell.addCommand(F("range"), F("<lower> <upper>"), rangeCommand);
    shell.attach(terminal);
}

//////////////////////////////////////////////////////////////////////////////
void loop() {
    // Should get:
    // TestRunner summary:
    //   <n> passed, <n> failed, <n> skipped, <n> timed out, out of <n> test(s).
    aunit::TestRunner::run();
}
