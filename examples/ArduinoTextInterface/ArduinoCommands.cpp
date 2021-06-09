// ArduinoCommands.cpp
// support for controlling Arduino pins through a serial command line interface
//
//
////////////////////////////////////////////////////////////////////////////////
#include <Arduino.h>
#include <SimpleSerialShell.h>
#include "ArduinoCommands.h"
#include <Streaming.h>

// see Arduino.h and pins_arduino.h for useful declarations.

////////////////////////////////////////////////////////////////////////////////
int badArgCount( char * cmdName )
{
    shell.print(cmdName);
    shell.println(F(": bad arg count"));
    return -1;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// int <--> symbolic translations
// declare it in PROGMEM so strings are not copied to RAM
//
// NOTE: PROGMEM access to values in this struct
// is not as simple as it would be in RAM.
//
struct lookupVals
{
    const char * PROGMEM name_pp;
    //const __FlashStringHelper *name;
    int val;
    String getName(void) const
    {
        auto nameInProgmem = &(name_pp);
        __FlashStringHelper * fshName
            = (__FlashStringHelper *)pgm_read_ptr(nameInProgmem);

        return String(fshName);
    };
    int getVal(void) const
    {
        auto valInProgmem = &(val);
        return pgm_read_word(valInProgmem);
    }
};

////////////////////////////////////////////////////////////////////////////////
int lookup(const char * aName, const lookupVals entries[])
{
    int i = 0;
    String name(aName);
    Serial << F("looking up \"") << name << F("\"") << endl;
    for (; entries[i].name_pp; i++)
    {
        Serial << F(" i = ") << i
               << F(" comparing against ") << entries[i].getName() << endl;
        //if (strncasecmp(aName, entries[i].name, 20) == 0) {
        if (name.equalsIgnoreCase(entries[i].getName())) {
            auto aVal = entries[i].getVal();
            Serial << F("found entry: ") << aVal << endl;
            return aVal;
            break;
        }
    }
    auto v = entries[0].getVal();
    Serial << "giving up; using" << v << endl;
    return v;
}

////////////////////////////////////////////////////////////////////////////////
//const char * reverseLookup(int aVal, const lookupVals entries[])
String reverseLookup(int aVal, const lookupVals entries[])
{
    int i = 0;
    for (; entries[i].name_pp; i++)
    {
        if (aVal == entries[i].val) {
            return entries[i].getName();
            break;
        }
    }
    return String(F("(unrecognized value)"));
}

////////////////////////////////////////////////////////////////////////////////
static const char input_s[] PROGMEM = "input";
static const char output_s[] PROGMEM = "output";
static const char pullup_s[] PROGMEM = "pullup";
static const lookupVals modes[] PROGMEM = {
    {input_s, INPUT},
    {output_s, OUTPUT},
    {pullup_s, INPUT_PULLUP},
    {NULL, INPUT} // default
};

int setPinMode(int argc, char **argv)
{
    if (argc == 3)
    {
        auto pin = atoi(argv[1]);
        auto mode = lookup(argv[2], modes);

        pinMode(pin, mode);
        return EXIT_SUCCESS;
    }

    return badArgCount(argv[0]);
}

////////////////////////////////////////////////////////////////////////////////
int analogRead(int argc, char **argv)
{
    if (argc == 2)
    {
        int pin = atoi(argv[1]);
        if (pin < 0 || pin > (int) NUM_ANALOG_INPUTS)
        {
            shell.print(F("pin "));
            shell.print(pin);
            shell.println(F(" does not look like an analog pin"));
        }
        auto val = analogRead(pin);
        shell.println(val);
        return EXIT_SUCCESS;
    }

    return badArgCount(argv[0]);
}

#ifndef ARDUINO_ARCH_ESP32
////////////////////////////////////////////////////////////////////////////////
int analogWrite(int argc, char **argv)
{
    if (argc == 3)
    {
        int pin = atoi(argv[1]);
        if (!digitalPinHasPWM(pin))
        {
            shell.print(F("pin "));
            shell.print(pin);
            shell.println(F(" does not look like an analog output"));
        }
        int val = atoi(argv[2]);

        analogWrite(pin, val);
        return EXIT_SUCCESS;
    }

    return badArgCount(argv[0]);
}
#endif

////////////////////////////////////////////////////////////////////////////////
const char low_s[] PROGMEM = "low";
const char high_s[] PROGMEM = "high";
const char zero_s[] PROGMEM = "0";
const char one_s[] PROGMEM = "1";
const lookupVals digLevels[] PROGMEM = {
    {low_s, LOW},
    {high_s, HIGH},
    {zero_s, LOW},
    {one_s, HIGH},
    {NULL, LOW} // default
};
////////////////////////////////////////////////////////////////////////////////
int digitalWrite(int argc, char **argv)
{
    if (argc == 3)
    {
        int pin = atoi(argv[1]);
        if (pin < 0 || pin >= (int) NUM_DIGITAL_PINS)
        {
            shell.print(F("pin "));
            shell.print(pin);
            shell.println(F(" does not look like a digital pin"));
        }
        auto level = lookup(argv[2], digLevels);

        digitalWrite(pin, level);
        return EXIT_SUCCESS;
    }

    return badArgCount(argv[0]);
}

////////////////////////////////////////////////////////////////////////////////
int digitalRead(int argc, char **argv)
{
    if (argc == 2)
    {
        int pin = atoi(argv[1]);
        if (pin < 0 || pin >= (int) NUM_DIGITAL_PINS)
        {
            shell.print(F("pin "));
            shell.print(pin);
            shell.println(F(" does not look like a digital pin"));
        }
        auto val = digitalRead(pin);
        shell.print(val);
        shell.print(F(" "));
        auto valName = reverseLookup(val, digLevels);
        shell.println(valName);

        return EXIT_SUCCESS;
    }

    return badArgCount(argv[0]);
}


#ifndef ARDUINO_ARCH_ESP32
////////////////////////////////////////////////////////////////////////////////
int doTone(int argc, char **argv)
{
    if (argc < 3 || argc > 4)
    {
        return badArgCount(argv[0]);
    }
    int pin = atoi(argv[1]);
    int freq = atoi(argv[2]);
    if (argc == 3)
    {
        tone(pin, freq);
    } else {
        int duration = atoi(argv[4]);
        tone(pin, freq, duration);
    }

    return EXIT_SUCCESS;
}
#endif

#ifndef ARDUINO_ARCH_ESP32
////////////////////////////////////////////////////////////////////////////////
int doNoTone(int argc, char **argv)
{
    if (argc != 2)
    {
        return badArgCount(argv[0]);
    }
    int pin = atoi(argv[1]);
    noTone(pin);

    return EXIT_SUCCESS;
}
#endif

//#define USE_COMMAND_TABLE
#define USE_ADD_COMMAND
//#define PARSE_IT_YOURSELF

#ifdef USE_COMMAND_TABLE

static const char PROGMEM spm[] = "setpinmode";
static const char PROGMEM digwrite[] = "digitalwrite";
static const char PROGMEM digread[] = "digitalread";
static const char PROGMEM anaread[] = "analogread";

static const PROGMEM SimpleSerialShell::CommandEntry commandTable[] = {
    { spm, setPinMode },
    { digwrite, digitalWrite },
    { digread, digitalRead },
    { anaread, analogRead },
    //{NULL, NULL }
};

#endif

#ifdef PARSE_IT_YOURSELF

bool matches(const char * arg, const __FlashStringHelper * command)
{
    int comparison = strncasecmp_P(arg, (const char *) command, SIMPLE_SERIAL_SHELL_BUFSIZE);
    return comparison == 0;

    //String anArg(arg);
    //String myCommand(command);
    //return anArg.equalsIgnoreCase(myCommand);
}
int doItYourself(int argcRaw, char **argvRaw)
{
    if (argcRaw < 2)
        return -1;
    int argc = argcRaw - 1;
    char **argv = argvRaw;
    argv++;

    //String command(argvRaw[1]);
    const char *command = argvRaw[1];

    // support help, too?
    if ( matches(command, F("setpinmode")))
    {
        return setPinMode(argc, argv);
    }
    if ( matches(command, F("digitalwrite")))
    {
        return digitalWrite(argc, argv);
    }
    if ( matches(command, F("digitalread")))
    {
        return digitalRead(argc, argv);
    }
    if ( matches(command, F("analogread")))
    {
        return analogRead(argc, argv);
    }
    shell.print(F("command \""));
    shell.print( command );
    shell.println(F("\" not found"));
    shell.println(F("See " __FILE__ " doItYourself() for syntax"));
    return -1;
}
#endif

////////////////////////////////////////////////////////////////////////////////
int addArduinoCommands(SimpleSerialShell & shell)
{
#ifdef USE_COMMAND_TABLE
    shell.addCommandTable(commandTable, sizeof(commandTable));
#endif

#ifdef USE_ADD_COMMAND
    shell.addCommand(F("setpinmode"), setPinMode);
    shell.addCommand(F("digitalwrite"), digitalWrite);
    shell.addCommand(F("digitalread"), digitalRead);
    shell.addCommand(F("analogread"), analogRead);
#endif

#ifdef PARSE_IT_YOURSELF
    shell.addCommand(F("doit"), doItYourself);
#endif

#ifndef ARDUINO_ARCH_ESP32
    shell.addCommand(F("analogwrite"), analogWrite);
    shell.addCommand(F("tone"), doTone);
    shell.addCommand(F("notone"), doNoTone);
#endif

    return EXIT_SUCCESS;
}
