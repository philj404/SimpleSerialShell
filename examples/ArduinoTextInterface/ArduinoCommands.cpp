// ArduinoCommands.cpp
// support for controlling Arduino pins through a serial command line interface
//
//
////////////////////////////////////////////////////////////////////////////////
#include <Arduino.h>
#include <SimpleSerialShell.h>
#include "ArduinoCommands.h"
//#include <Streaming.h>

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
// But const strings in RAM can take significant space.
//
struct lookupVals
{
#ifdef AVR
    const char * PROGMEM name_pp;
#else
    const char * name_pp;
#endif
    int val;
    __FlashStringHelper * getNamePtr(void) const
    {
        auto nameInProgmem = &(name_pp);
        __FlashStringHelper * fshName
            = (__FlashStringHelper *)pgm_read_ptr(nameInProgmem);
        return fshName;
    }
    String getName(void) const
    {
        auto fshName = getNamePtr();
        return String(fshName);
    };
    int getValue(void) const
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
    //Serial << F("looking up \"") << name << F("\"") << endl;
    for (; entries[i].getNamePtr(); i++)
    {
        //Serial << F(" i = ") << i
        //       << F(" comparing against ") << entries[i].getName() << endl;
        if (name.equalsIgnoreCase(entries[i].getName())) {
            auto aVal = entries[i].getValue();
            //Serial << F("found entry: ") << aVal << endl;
            return aVal;
            break;
        }
    }
    auto v = entries[0].getValue();
    //Serial << "giving up; using" << v << endl;
    return v;
}

////////////////////////////////////////////////////////////////////////////////
String reverseLookup(int aVal, const lookupVals entries[])
{
    //Serial << F("Looking up ") << aVal << endl;
    for (int i = 0; entries[i].getNamePtr(); i++)
    {
        //Serial << F(" i = ") << i
        //       << F(" comparing against ") << entries[i].getName() << endl;
        if (aVal == entries[i].getValue()) {
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
    {NULL, INPUT} // end of list
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
    {NULL, LOW} // end of list
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

#define WITH_HELPINFO
#ifdef WITH_HELPINFO
#define ADD_COMMAND(name, argumentHints, function)\
	shell.addCommand(F(name " " argumentHints),function)
#else
    // if there is no room for a help hint,
    // shorten the string to just the command
#define ADD_COMMAND(name, argumentHints, function)\
	shell.addCommand(F(name),function)
#endif

////////////////////////////////////////////////////////////////////////////////
int addArduinoCommands(SimpleSerialShell & shell)
{
    //shell.addCommand(F("setpinmode" ), setPinMode);
    //shell.addCommand(F("setpinmode pinNumber {input|output|pullup}"), setPinMode);
    //
    ADD_COMMAND("setpinmode", "pinNumber {input|output|pullup}", setPinMode);
    ADD_COMMAND("digitalwrite", "pinNumber {low|high|0|1}", digitalWrite);
    ADD_COMMAND("digitalread", "pinNumber", digitalRead);
    ADD_COMMAND("analogread", "pinNumber", analogRead);

#ifndef ARDUINO_ARCH_ESP32
    ADD_COMMAND("analogwrite", "pinNumber value", analogWrite);
    ADD_COMMAND("tone", "pinNumber freqHz [durationMillisec]", doTone);
    ADD_COMMAND("notone", "pinNumber", doNoTone);
#endif

    return EXIT_SUCCESS;
}
