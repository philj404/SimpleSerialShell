// basic example testing of the command parser.
#include <SimpleSerialShell.h>

/////////////////////////////////////////////////////////////////////////////////////
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
void setup() {
    // put your setup code here, to run once:

    Serial.begin(115200);
    while (!Serial) {
        // wait for serial port to connect. Needed for native USB port only
        // AND you want to block until there's a connection
        // otherwise the shell can quietly drop output.
    }

    //example
    shell.attach(Serial);
    shell.addCommand(F("id?"), showID);

    showID();
    shell.println(F("Ready."));
    shell.println(F("Type 'help' for help"));
}

////////////////////////////////////////////////////////////////////////////////
void loop() {
    // put your main code here, to run repeatedly:

    shell.executeIfInput();
}
