// ArduinoCommands.cpp
// support for controlling Arduino pins through a serial command line interface
//
//
////////////////////////////////////////////////////////////////////////////////
#include <Arduino.h>
#include <SimpleSerialShell.h>
#include "ArduinoCommands.h"

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
//
struct lookupVals
{
  const char * name;
  int val;
};

////////////////////////////////////////////////////////////////////////////////
int lookup(const char * aName, const lookupVals entries[])
{
  int i = 0;
  for (; entries[i].name; i++)
  {
    if (strncasecmp(aName, entries[i].name, 20) == 0) {
      //return entries[i].val;
      break;
    }
  }
  return entries[i].val;
}

////////////////////////////////////////////////////////////////////////////////
const char * reverseLookup(int aVal, const lookupVals entries[])
{
  int i = 0;
  for (; entries[i].name; i++)
  {
    if (aVal == entries[i].val) {
      break;
    }
  }
  return entries[i].name;
}

////////////////////////////////////////////////////////////////////////////////
const lookupVals PROGMEM modes[] = {
  //{"input", INPUT},
  {"output", OUTPUT},
  {"pullup", INPUT_PULLUP},
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
const lookupVals PROGMEM digLevels[] = {
  {"high", HIGH},
  {NULL, LOW} // default
};
////////////////////////////////////////////////////////////////////////////////
int digitalWrite(int argc, char **argv)
{
  if (argc == 3)
  {
    int pin = atoi(argv[1]);
    if (pin < 0 || pin > NUM_DIGITAL_PINS)
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
    if (pin < 0 || pin > NUM_DIGITAL_PINS)
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

////////////////////////////////////////////////////////////////////////////////
int addArduinoCommands(SimpleSerialShell & shell)
{
  shell.addCommand(F("setpinmode"), setPinMode);
  shell.addCommand(F("digitalwrite"), digitalWrite);
  shell.addCommand(F("digitalread"), digitalRead);
  shell.addCommand(F("analogread"), analogRead);

#ifndef ARDUINO_ARCH_ESP32
  shell.addCommand(F("analogwrite"), analogWrite);
  shell.addCommand(F("tone"), doTone);
  shell.addCommand(F("notone"), doNoTone);
#endif

  return EXIT_SUCCESS;
}
