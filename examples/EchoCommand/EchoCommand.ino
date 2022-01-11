// basic example testing of the command parser.
#include <SimpleSerialShell.h>

////////////////////////////////////////////////////////////////////////////////
// Forgot what sketch was loaded to this board?
//
// Hint1: use the F() macro to keep const strings in FLASH and save RAM
// Hint2: "Compiler " "catenates consecutive "
//         "strings together"
//         (improves readability for very long strings)
//
int showID(int /*argc*/ = 0, char** /*argv*/ = NULL)
{
    shell.println(F( "Running " __FILE__ ", Built " __DATE__));
    return 0;
};


////////////////////////////////////////////////////////////////////////////////
// echo
// simple version of Unix style 'echo'
//
// example:  "echo Hello world"
//  prints:  "Hello world\n" to attached stream (Serial, Serial1 etc.).
//
////////////////////////////////////////////////////////////////////////////////
//
int echo(int argc, char **argv)
{
    auto lastArg = argc - 1;
    for ( int i = 1; i < argc; i++) {

        shell.print(argv[i]);

        if (i < lastArg)
            shell.print(F(" "));
    }
    shell.println();

    return EXIT_SUCCESS;
}


////////////////////////////////////////////////////////////////////////////////
// non blocking LED toggle
//
void toggleLED_nb(void)
{
    static auto lastToggle = millis();  // saved between calls
    auto now = millis();

    if (now - lastToggle > 1000)
    {
        // toggle
        digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
        lastToggle = now;
    }
}

////////////////////////////////////////////////////////////////////////////////
void setup() {
    // put your setup code here, to run once:

    pinMode(LED_BUILTIN, OUTPUT);

    //Serial.begin(9600); older default bit rate
    Serial.begin(115200);
    while (!Serial) {
        // wait for serial port to connect. Needed for native USB port only
        // AND you want to block until there's a connection
        // otherwise the shell can quietly drop output.
    }

    //example
    shell.attach(Serial);
    shell.addCommand(F("id?"), showID);

    // adds a basic "echo" command
    shell.addCommand(F("echo"), echo);

    // adds an "echo" command with argument hints for help
    // nice for complex commands
    shell.addCommand(F("echoWithHints <wordToEcho> ..."), echo);

    showID();
    Serial.println(F("Ready."));
}

// non blocking LED toggle
extern void toggleLED_nb(void);

////////////////////////////////////////////////////////////////////////////////
void loop() {
    // put your main code here, to run repeatedly:

    //shell->execute("echo Hello World");
    shell.executeIfInput();

    // show loop() is still running -- not waiting
    toggleLED_nb();
}
