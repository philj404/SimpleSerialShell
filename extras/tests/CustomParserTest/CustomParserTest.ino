// CustomParserTest.ino
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

using namespace aunit;

SimMonitor terminal;

void prepForTests(void)
{
    terminal.init();
    shell.resetBuffer();
}

//////////////////////////////////////////////////////////////////////////////
// test fixture to ensure clean initial and final conditions
//
class ShellTest: public TestOnce {
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

//////////////////////////////////////////////////////////////////////////////
// A demonstration of the alternate tokenizer feature.  Please
// note that this test runs with an independent shell and an 
// independent terminal in order to avoid any conflict with global
// state that appears elsewhere in this library.
//
static SimpleSerialShell shell2;
static SimMonitor terminal2;

/**
 * @brief Echoes the parameters of the function with comma-delimiting.
 * @return int 0 On success, non-zero on error.
 */
int echo2(int argc, char **argv) {
    auto lastArg = argc - 1;
    for ( int i = 1; i < argc; i++) {
        shell2.print(argv[i]);
        if (i < lastArg)
            shell2.print(F(" "));
    }
    shell2.println();
    return 0;
}

/**
 * @brief An alternate tokenizer that uses a comma-delimiter after the 
 *   initial token has been identified.
 */
char* commaTokenizer(char* str, const char*, char** saveptr) {
    // The first time we tokenize on space so that we can 
    // pick up the command.  Subsequent calls we tokenize on 
    // comma.
    if (str != 0) {
        return strtok_r(str, " ", saveptr);
    } else {
        return strtok_r(str, ",", saveptr);
    }
}

testF(ShellTest, altTokenizer) {

    const char* testCommand = "echo2 test1,test2,test3";

    // Basic sanity check using the current semantics
    terminal2.pressKeys(testCommand);
    assertFalse(shell2.executeIfInput());
    // Flush the user-entry
    terminal2.getline();
    terminal2.pressKey('\r');
    assertTrue(shell2.executeIfInput());
    // Everything comes back as a single token
    assertEqual(terminal2.getline(), "\r\ntest1,test2,test3\r\n");

    // Now plug in a new tokenizer that looks for commas instead of spaces.
    // This is the main point of this unit test.
    shell2.setTokenizer(commaTokenizer);

    // Send in the same command
    terminal2.pressKeys(testCommand);
    assertFalse(shell2.executeIfInput());
    // Flush the user-entry
    terminal2.getline();
    terminal2.pressKey('\r');
    assertTrue(shell2.executeIfInput());
    // Notice now that the three tokens were separated
    assertEqual(terminal2.getline(), "\r\ntest1 test2 test3\r\n");
}

//////////////////////////////////////////////////////////////////////////////
void setup() {
    ::delay(1000); // wait for stability on some boards to prevent garbage Serial
    Serial.begin(115200); // ESP8266 default of 74880 not supported on Linux
    while (!Serial); // for the Arduino Leonardo/Micro only

    // NB: Even though this command be being added to the shell2 instance, 
    // the list of commands is held in a static list so it will be visible
    // in the shell instance as well.  This is the reason why we are 
    // calling the command "echo2."
    shell2.addCommand(F("echo2"), echo2);
    shell2.attach(terminal2);
}

//////////////////////////////////////////////////////////////////////////////
void loop() {
    // Should get:
    // TestRunner summary:
    //   <n> passed, <n> failed, <n> skipped, <n> timed out, out of <n> test(s).
    aunit::TestRunner::run();
}
