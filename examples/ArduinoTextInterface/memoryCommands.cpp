// memoryCommands.cpp
// Basic support for reading Arduino RAM/PROGMEM/EEPROM data
// through a serial command line interface
//
////////////////////////////////////////////////////////////////////////////////
#include <Arduino.h>
#include <SimpleSerialShell.h>
#include "ArduinoCommands.h"
#ifdef AVR
#include <EEPROM.h>
#endif

static int checkSyntax(void)
{
    shell.println(F("check syntax"));
    return -18;
}

////////////////////////////////////////////////////////////////////////////////
long parseAsHex(const char *aValue)
{
    long value = -1;    // parse failure
    bool done = false;
    for (int i = 0; ((i < 8) && !done); i++) {
        int nextDigit = -1;
        char nextChar = tolower(aValue[i]);
        switch (nextChar) {
            case '0' ... '9':
                nextDigit = nextChar - '0';
                break;
            case 'a' ... 'f':
                nextDigit = nextChar - 'a' + 0xa;
                break;
            default:
                done = true;
                break;
        }
        if (nextDigit > -1) {
            if (value < 0) {
                value = nextDigit;  // first digit
            } else {
                value <<= 4;    // make room
                value |= nextDigit;
            }
        }
    }
    return value;
}

////////////////////////////////////////////////////////////////////////////////
void prettyPrintChars(int lineNo, const char *chars, int numChars)
{
    shell.print(lineNo, HEX);
    for (int j = 0; j < numChars; j++) {  // hex values
        unsigned char b = chars[j];
        shell.print(F(" "));
        if(b < 0x10) {
            shell.print(F("0"));
        }
        shell.print(b, HEX);
    }
    shell.print(F(" "));
    for (int j = 0; j < numChars; j++) {  // text values
        char b = chars[j];
        bool printIt = isPrintable(b);
        shell.print(printIt ? b : '.');
    }
    shell.println();
}

static const int rowSize = 0x10;

////////////////////////////////////////////////////////////////////////////////
// displaying any kind of memory is pretty similar, even if access is different
//
int dumpAMemory(int argc, char **argv)
{
    const char dumperNameStarts = tolower(argv[0][0]);
    bool dumpingRAM = (dumperNameStarts == 'r');
#ifdef AVR
    bool dumpingEEPROM = (dumperNameStarts == 'e');
#endif
    bool dumpingPROGMEM = (dumperNameStarts == 'p');

    int begin = 0;
    if (argc > 1) {
        begin = parseAsHex(argv[1]);    // start address
        if (begin < 0) {
            return checkSyntax();
        }
    }

    int end = begin + rowSize;
#ifdef AVR
    if (dumpingEEPROM) {
        end = (unsigned) EEPROM.length();   // default do all EEPROM
    }
#endif
    if (argc > 2) {
        int numBytes = parseAsHex(argv[2]);
        if (numBytes < 0) {
            return checkSyntax();
        }
        end = begin + numBytes;
    }

    char aLine[rowSize];
    for (int i = begin; i < end; i += rowSize) {
        for (int j = 0; j < rowSize; j++) {
            char b = 0;
            if (dumpingPROGMEM) {
                b = pgm_read_byte( (void *) (i + j));
            }
            if (dumpingRAM) {
                const char *allRam = (char *) i;
                b = allRam[j];
            }
#ifdef AVR
            if (dumpingEEPROM) {
                b = EEPROM.read(i + j);
            }
#endif
            aLine[j] = b;
        }
        prettyPrintChars(i, aLine, rowSize);
    }
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
int addMemoryCommands(SimpleSerialShell & shell)
{
#ifdef AVR
    shell.addCommand(F("eeprom? [<beginHex> [<numBytesHex>]]"), dumpAMemory);
#endif
    shell.addCommand(F("ram? [<beginHex> [<numBytesHex>]]"), dumpAMemory);
    shell.addCommand(F("progmem? [<beginHex> [<numBytesHex>]]"), dumpAMemory);
    return 0;
}
