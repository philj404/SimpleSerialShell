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
#include "SimulatedStream.h"
#include <SimpleSerialShell.h>

using namespace aunit;

// some platforms ouput line endings differently.
#define NEW_LINE "\r\n"
//#define NEW_LINE "\n"
#define COMMAND_PROMPT NEW_LINE "> "

// A mock of the Arduino Serial stream
static SimulatedStream<128> terminal;

void prepForTests(void)
{
    terminal.init();
    shell.resetBuffer();
}

//////////////////////////////////////////////////////////////////////////////
// test fixture to ensure clean initial and final conditions
//
class CustomParserTest: public TestOnce {
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
// A demonstration of the alternate tokenizer feature. 

/**
 * @brief Echoes the parameters of the function with comma-delimiting.
 * @return int 0 On success, non-zero on error.
 */
int echo(int argc, char **argv) {
    auto lastArg = argc - 1;
    for ( int i = 1; i < argc; i++) {
        shell.print(argv[i]);
        if (i < lastArg)
            shell.print(F(" "));
    }
    shell.println();
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

testF(CustomParserTest, altTokenizer) {

    const char* testCommand = "echo test1,test2,test3";

    // Basic sanity check using the current semantics
    terminal.pressKeys(testCommand);
    assertFalse(shell.executeIfInput());
    // Flush the user-entry
    terminal.getline();
    terminal.pressKey('\r');
    assertTrue(shell.executeIfInput());
    // Everything comes back as a single token
    assertEqual(terminal.getline(),  (NEW_LINE "test1,test2,test3" COMMAND_PROMPT));

    // Now plug in a new tokenizer that looks for commas instead of spaces.
    // This is the main point of this unit test.
    shell.setTokenizer(commaTokenizer);

    // Send in the same command
    terminal.pressKeys(testCommand);
    assertFalse(shell.executeIfInput());
    // Flush the user-entry
    terminal.getline();
    terminal.pressKey('\r');
    assertTrue(shell.executeIfInput());
    // Notice now that the three tokens were separated
    assertEqual(terminal.getline(),  (NEW_LINE "test1 test2 test3" COMMAND_PROMPT));
}

//////////////////////////////////////////////////////////////////////////////
// ... so which sketch is this?
int showID(int /*argc*/ = 0, char ** /*argv*/ = NULL)
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

    shell.addCommand(F("echo"), echo);
    shell.attach(terminal);
}

//////////////////////////////////////////////////////////////////////////////
void loop() {
    // Should get:
    // TestRunner summary:
    //   <n> passed, <n> failed, <n> skipped, <n> timed out, out of <n> test(s).
    aunit::TestRunner::run();
}
