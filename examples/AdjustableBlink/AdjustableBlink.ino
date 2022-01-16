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
    shell.println(F( "Running " __FILE__ ",\nBuilt " __DATE__));
    return 0;
};


int togglePeriod = 1000;

////////////////////////////////////////////////////////////////////////////////
// non blocking LED toggle
//
void toggleLED_nb(void)
{
    static auto lastToggle = millis();  // saved between calls
    auto now = millis();

    if (now - lastToggle > (unsigned int) (togglePeriod / 2) )
    {
        // toggle
        digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
        lastToggle = now;
    }
}

////////////////////////////////////////////////////////////////////////////////
int setTogglePeriod(int argc, char **argv)
{
    if (argc != 2) {
        shell.println("bad argument count");
        return -1;
    }
    togglePeriod = atoi(argv[1]);
    shell.print("Setting LED toggle period to ");
    shell.print(togglePeriod);
    shell.println("ms");

    return EXIT_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////
int getTogglePeriod(int /*argc*/ , char ** /*argv*/ )
{
    shell.print("LED toggle period is ");
    shell.print(togglePeriod);
    shell.println("ms");

    return EXIT_SUCCESS;
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

    //basic command
    //shell.addCommand(F("setTogglePeriod"), setTogglePeriod);
    // command with hint for help
    shell.addCommand(F("setTogglePeriod <milliseconds>"), setTogglePeriod);


    shell.addCommand(F("getTogglePeriod"), getTogglePeriod);

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
