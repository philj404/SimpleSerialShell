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
    terminal.init();
    shell.resetBuffer();
}

//////////////////////////////////////////////////////////////////////////////
// shell.execute( string ) works?
test(execute) {
    prepForTests();

    int response = shell.execute("echo hello world");
    assertEqual(response, 0);
    int errNo = shell.lastErrNo();
    assertEqual(errNo, 0);      // OK or no errors

    assertEqual(terminal.getline(), "hello world\r\n");

    response = shell.execute("sum 1 2 3");
    assertEqual(response, 6);
    errNo = shell.lastErrNo();
    assertEqual(errNo, 6);      // sum(...) returned 6
};

//////////////////////////////////////////////////////////////////////////////
// shell.execute( string ) fails for missing command?
test(missingCommand) {
    prepForTests();

    int response = shell.execute("echoNOT hello world");
    assertEqual(response, -1);
    int errNo = shell.lastErrNo();
    assertEqual(errNo, -1);      // OK or no errors

    assertEqual(terminal.getline(), "\"echoNOT\": -1: command not found\r\n");
};

//////////////////////////////////////////////////////////////////////////////
// shell.executeIfInput() from stream works?
test(executeIfInput) {
    prepForTests();

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

    assertEqual(terminal.getline(), "\r\nhello world\r\n");
};

//////////////////////////////////////////////////////////////////////////////
//  erase input line?
test(cancelLine) {
    prepForTests();

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
    assertEqual(aLine, "XXX\r\n");

    const char* echoCmd = "echo aWord";
    terminal.pressKeys(echoCmd);
    response = shell.executeIfInput();
    aLine = terminal.getline();    // flush output
    terminal.pressKey('\r');
    response = shell.executeIfInput();

    assertTrue(response);
    errNo = shell.lastErrNo();
    assertEqual(errNo, 0);    // OK or no errors

    assertEqual(terminal.getline(), "\r\naWord\r\n");
};

//////////////////////////////////////////////////////////////////////////////
//  retype input line
test(retypeLine) {
    prepForTests();

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
    assertEqual(aLine, "\r\necho howdy doodie");

    terminal.pressKey('\r');
    response = shell.executeIfInput();

    assertTrue(response);
    errNo = shell.lastErrNo();
    assertEqual(errNo, 0);    // OK or no errors

    assertEqual(terminal.getline(), "\r\nhowdy doodie\r\n");
};

//////////////////////////////////////////////////////////////////////////////
// shell backspace from stream works?
test(backspace) {
    prepForTests();

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

    assertEqual(terminal.getline(), "\r\nhello world\r\n");
}

//////////////////////////////////////////////////////////////////////////////
// shell DEL key from stream works?
test(delete) {
    prepForTests();

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

    assertEqual(terminal.getline(), "\r\nhello world\r\n");
}

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
    //   <n> passed, <n> failed, <n> skipped, <n> timed out, out of <n> test(s).
    aunit::TestRunner::run();
}
