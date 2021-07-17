#include <Arduino.h>
#include "ArduinoCommands.h"

#if defined(AVR)

#include "MemoryUsageAVR.h"
#include <SimpleSerialShell.h>

using namespace MU_AVR;

////////////////////////////////////////////////////////////////////////////////
// probe memory usage
// NOTE this function call needs/uses a little more memory/stack
//
int memoryUsageProbe(int argc = 0, char **argv = NULL)
{
    STACK_COMPUTE // call as needed to probe current stack depth

    bool all = false;
    bool graphical = false;
    bool heap = false;
    bool stack = false;
    bool free = false;

    if (argc < 2)
    {   // default
        graphical = true;
    }
    else
    {
        for (char *flagPtr = argv[1]; *flagPtr != 0; flagPtr++)
        {
            switch (*flagPtr)
            {
                case 'a': // all
                    all = true;
                    break;
                case 'h': // heap
                    heap = true;
                    break;
                case 'g': // graphical
                    graphical = true;
                    break;
                case 's':
                    stack = true;
                    break;
                case 'f':
                    free = true;
                    break;
                default:
                    break;
            }
        }
    }

// helpers to print a line of args to shell
#define PRINT2(a, b) shell.print((a)); shell.println((b));
#define PRINT2HEX(a, b) shell.print((a)); shell.print((b)); shell.print(F("/0x")); shell.println((b),HEX);

    if (all)
    {
        PRINT2HEX(F("Data start: "), getDataStart());
    }
    if (heap || all)
    {
        PRINT2HEX(F("Heap start: "), getHeapStart());
        PRINT2HEX(F("Heap end: "), getHeapEnd());
        PRINT2(F("Heap size: "), getHeapSize());
    }
    if (stack || all)
    {
        PRINT2HEX(F("Stack start: "), getStackStart());
        PRINT2HEX(F("Stack end: "), getMemoryEnd());
        PRINT2(F("Stack size: "), getStackSize());
        PRINT2(F("Stack Max Size (at STACK_COMPUTE): "), maxStackSize);
        PRINT2(F("    num STACK_COMPUTE checkpoints: "), numStackComputeCalls);
    }
    if (free || all)
    {
        PRINT2(F("Free Ram Size: "), getFreeRam());
    }
    if (graphical || all)
    {
        SRamDisplay();
    }
    return 0;
};

int addMemoryUsageCommands(SimpleSerialShell &shell)
{
    STACK_COMPUTE
    shell.addCommand(F("memory?"), memoryUsageProbe);
    return 0;
}
#else
// otherwise not using the MemoryUsage library.
#endif
