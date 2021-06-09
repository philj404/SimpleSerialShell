// EEPROMCommands.cpp
// Basic support for reading/writing Arduino EEPROM data
// through a serial command line interface
//
////////////////////////////////////////////////////////////////////////////////
#include <Arduino.h>
#include <SimpleSerialShell.h>
#include "ArduinoCommands.h"
#include <EEPROM.h>
#include <Streaming.h>

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

#ifdef SPECIALIZED_DUMPS
////////////////////////////////////////////////////////////////////////////////
int dumpEEPROM(int argc, char **argv)
{
    char aLine[rowSize];
    int begin = 0;

    if (argc > 1) {
        begin = parseAsHex(argv[1]);    // start address
        if (begin < 0) {
            return checkSyntax();
        }
    }

    int end = (unsigned) EEPROM.length();
    if (argc > 2) {
        int numBytes = parseAsHex(argv[2]);
        if (numBytes < 0) {
            return checkSyntax();
        }
        end = begin + numBytes;
    }
    for (int i = begin; i < end; i += rowSize) {
        for (int j = 0; j < rowSize; j++) {  // hex values
            char b = EEPROM.read(i + j);
            aLine[j] = b;
        }
        prettyPrintChars(i, aLine, rowSize);
    }
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
int dumpRam(int argc, char **argv)
{
    char aLine[rowSize];
    int begin = 0;

    if (argc > 1) {
        begin = parseAsHex(argv[1]);    // start address
        if (begin < 0) {
            return checkSyntax();
        }
    }

    int end = begin + rowSize;
    if (argc > 2) {
        int numBytes = parseAsHex(argv[2]);
        if (numBytes < 0) {
            return checkSyntax();
        }
        end = begin + numBytes;
    }
    for (int i = begin; i < end; i += rowSize) {
        char *allRam = (char *) i;
        for (int j = 0; j < rowSize; j++) {  // hex values
            char b = allRam[j];
            aLine[j] = b;
        }
        prettyPrintChars(i, aLine, rowSize);
    }
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
int dumpProgmem(int argc, char **argv)
{
    char aLine[rowSize];
    int begin = 0;

    if (argc > 1) {
        begin = parseAsHex(argv[1]);    // start address
        if (begin < 0) {
            return checkSyntax();
        }
    }

    int end = begin + rowSize;
    if (argc > 2) {
        int numBytes = parseAsHex(argv[2]);
        if (numBytes < 0) {
            return checkSyntax();
        }
        end = begin + numBytes;
    }
    for (int i = begin; i < end; i += rowSize) {
        for (int j = 0; j < rowSize; j++) {
            char b = pgm_read_byte(i + j);
            aLine[j] = b;
        }
        prettyPrintChars(i, aLine, rowSize);
    }
    return 0;
}
#endif

////////////////////////////////////////////////////////////////////////////////
// displaying any kind of memory is pretty similar, even if access is different
//
int dumpAMemory(int argc, char **argv)
{
    const char dumperNameStarts = tolower(argv[0][0]);
    bool dumpingRAM = (dumperNameStarts == 'r');
    bool dumpingEEPROM = (dumperNameStarts == 'e');
    bool dumpingPROGMEM = (dumperNameStarts == 'p');

    int begin = 0;
    if (argc > 1) {
        begin = parseAsHex(argv[1]);    // start address
        if (begin < 0) {
            return checkSyntax();
        }
    }

    int end = begin + rowSize;
    if (dumpingEEPROM) {
        end = (unsigned) EEPROM.length();   // default do all EEPROM
    }
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
                b = pgm_read_byte(i + j);
            }
            if (dumpingRAM) {
                const char *allRam = (char *) i;
                b = allRam[j];
            }
            if (dumpingEEPROM) {
                b = EEPROM.read(i + j);
            }
            aLine[j] = b;
        }
        prettyPrintChars(i, aLine, rowSize);
    }
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
int addMemoryCommands(SimpleSerialShell & shell)
{
    shell.addCommand(F("eeprom?"), dumpAMemory);
    shell.addCommand(F("ram?"), dumpAMemory);
    shell.addCommand(F("progmem?"), dumpAMemory);
    return 0;
}
