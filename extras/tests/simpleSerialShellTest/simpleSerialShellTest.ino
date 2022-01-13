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

using namespace aunit;

SimMonitor terminal;

#define END_LINE "\r\n"
#define COMMAND_PROMPT END_LINE "> "

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
// shell.execute( string ) works?
testF(ShellTest, execute) {

    int response = shell.execute("echo hello world");
    assertEqual(response, 0);
    int errNo = shell.lastErrNo();
    assertEqual(errNo, 0);      // OK or no errors

    assertEqual(terminal.getline(), "hello world" END_LINE); // no prompt

    response = shell.execute("sum 1 2 3");
    assertEqual(response, 6);
    errNo = shell.lastErrNo();
    assertEqual(errNo, 6);      // sum(...) returned 6
};

//////////////////////////////////////////////////////////////////////////////
// shell.execute( string ) fails for missing command?
testF(ShellTest, missingCommand) {

    int response = shell.execute("echoNOT hello world");
    assertEqual(response, -1);
    int errNo = shell.lastErrNo();
    assertEqual(errNo, -1);      // OK or no errors

    assertEqual(terminal.getline(), "\"echoNOT\": -1: command not found" END_LINE);
};

//////////////////////////////////////////////////////////////////////////////
// shell.executeIfInput() from stream works?
testF(ShellTest, executeIfInput) {

    const char* echoCmd = "echo hello world";
    bool response = false;
    int errNo = 0;

    for (int i = 0; echoCmd[i] != '\0'; i++) {
        auto aKey = echoCmd[i];
        terminal.pressKey(echoCmd[i]);
        response = shell.executeIfInput();
        assertFalse(response);

        char echoed = (char) terminal.getOutput(); // typed keys echoed back?
        assertEqual(echoed, aKey);
    }

    terminal.pressKey('\r');
    response = shell.executeIfInput();
    assertTrue(response);
    errNo = shell.lastErrNo();
    assertEqual(errNo, 0);    // OK or no errors

    assertEqual(terminal.getline(), END_LINE "hello world" COMMAND_PROMPT);
};

//////////////////////////////////////////////////////////////////////////////
//  erase input line?
testF(ShellTest, cancelLine) {

    const char* badCmd = "BADCOMMAND BADCOMMAND";
    bool response = false;
    int errNo = 0;

    terminal.pressKeys(badCmd);
    response = shell.executeIfInput();
    assertFalse(response);
    String aLine = terminal.getline();  // flush output

    terminal.pressKey(0x15);   // CTRL('U')
    response = shell.executeIfInput();
    aLine = terminal.getline();
    assertEqual(aLine, "XXX" END_LINE);

    const char* echoCmd = "echo aWord";
    terminal.pressKeys(echoCmd);
    response = shell.executeIfInput();
    aLine = terminal.getline();    // flush output
    terminal.pressKey('\r');
    response = shell.executeIfInput();

    assertTrue(response);
    errNo = shell.lastErrNo();
    assertEqual(errNo, 0);    // OK or no errors

    assertEqual(terminal.getline(), END_LINE "aWord" COMMAND_PROMPT);
};

//////////////////////////////////////////////////////////////////////////////
//  retype input line
testF(ShellTest, retypeLine) {

    const char* echoCmd = "echo howdy doodie";
    bool response = false;
    int errNo = 0;

    terminal.pressKeys(echoCmd);
    response = shell.executeIfInput();
    assertFalse(response);
    String aLine = terminal.getline();  // flush output

    terminal.pressKey(0x12);   // CTRL('R') retype line
    response = shell.executeIfInput();
    aLine = terminal.getline();
    assertEqual(aLine, END_LINE "echo howdy doodie");

    terminal.pressKey('\r');
    response = shell.executeIfInput();

    assertTrue(response);
    errNo = shell.lastErrNo();
    assertEqual(errNo, 0);    // OK or no errors

    assertEqual(terminal.getline(), END_LINE "howdy doodie" COMMAND_PROMPT);
};

//////////////////////////////////////////////////////////////////////////////
// shell backspace from stream works?
testF(ShellTest, backspace) {

    char eraseChar = '\b';
    const char* echoCmd = "ech3";
    bool response = false;
    int errNo = 0;

    for (int i = 0; echoCmd[i] != '\0'; i++) {
        char aKey = echoCmd[i];
        terminal.pressKey(aKey);
        response = shell.executeIfInput();
        assertFalse(response);

        char echoed = (char) terminal.getOutput(); // typed keys echoed back?
        assertEqual(echoed, aKey);
    }

    terminal.pressKey(eraseChar);
    response = shell.executeIfInput();
    char echoed = (char) terminal.getOutput();
    assertEqual((int) echoed, (int) '\b'); // back up
    echoed = (char) terminal.getOutput();
    assertEqual( echoed, ' '); // erase 'typo' char
    echoed = (char) terminal.getOutput();
    assertEqual((int) echoed, (int) '\b'); // back up (cursor on whitespace)

    const char* echoCmdFinish = "o hello world";
    for (int i = 0; echoCmdFinish[i] != '\0'; i++) {
        auto aKey = echoCmdFinish[i];
        terminal.pressKey(aKey);
        response = shell.executeIfInput();
        assertFalse(response);

        char echoed = (char) terminal.getOutput(); // typed keys echoed back?
        assertEqual(echoed, aKey);
    }

    terminal.pressKey('\r');
    response = shell.executeIfInput();
    assertTrue(response);
    errNo = shell.lastErrNo();
    assertEqual(errNo, 0);    // OK or no errors

    assertEqual(terminal.getline(), END_LINE "hello world" COMMAND_PROMPT);
}

//////////////////////////////////////////////////////////////////////////////
// shell DEL key from stream works?
testF(ShellTest, delete) {

    char eraseChar = (char) 127;    // DEL key
    const char* echoCmd = "ech3";
    bool response = false;
    int errNo = 0;

    terminal.pressKeys(echoCmd);
    response = shell.executeIfInput();
    assertFalse(response);
    auto echoed = terminal.getline(); // typed keys echoed back?
    assertEqual(echoCmd, echoed);

    terminal.pressKey(eraseChar);
    response = shell.executeIfInput();
    echoed = terminal.getline();
    assertEqual("\b \b", echoed); // back up, erase typo, back up

    const char* echoCmdFinish = "o hello world";

    terminal.pressKeys(echoCmdFinish);
    response = shell.executeIfInput();
    assertEqual(terminal.getline(), echoCmdFinish);

    terminal.pressKey('\r');
    response = shell.executeIfInput();
    assertTrue(response);
    errNo = shell.lastErrNo();
    assertEqual(errNo, 0);    // OK or no errors

    assertEqual(terminal.getline(), END_LINE "hello world" COMMAND_PROMPT);
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
    shell.attach(terminal);
    shell.addCommand(F("id?"), showID);

    addTestCommands();
}

//////////////////////////////////////////////////////////////////////////////
void loop() {
    // Should get:
    // TestRunner summary:
    //   <n> passed, <n> failed, <n> skipped, <n> timed out, out of <n> test(s).
    aunit::TestRunner::run();
}
