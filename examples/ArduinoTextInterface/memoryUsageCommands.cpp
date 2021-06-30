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

    // NOTE: MEMORY_PRINT_XXX library macros all print to Serial
#define PRINT2(a, b) shell.print((a)); shell.println((b));
#define PRINT2HEX(a, b) shell.print((a)); shell.print((b)); shell.print(F("/0x")); shell.println((b),HEX);


    if (all)
    {
        //MEMORY_PRINT_START
        PRINT2HEX(F("Data start: "), getDataStart());
    }
    if (heap || all)
    {
        //MEMORY_PRINT_HEAPSTART
        PRINT2HEX(F("Heap start: "), getHeapStart());
        //MEMORY_PRINT_HEAPEND
        PRINT2HEX(F("Heap end: "), getHeapEnd());
        //MEMORY_PRINT_HEAPSIZE
        PRINT2(F("Heap size: "), getHeapSize());
    }
    if (stack || all)
    {
        //MEMORY_PRINT_STACKSTART
        PRINT2HEX(F("Stack start: "), getStackStart());
        //MEMORY_PRINT_END
        PRINT2HEX(F("Stack end: "), getMemoryEnd());
        //MEMORY_PRINT_STACKSIZE
        PRINT2(F("Stack size: "), getStackSize());
        //STACK_PRINT
        PRINT2(F("Stack Maximum Size (Instrumentation method): "), mu_stack_size);
    }
    if (free || all)
    {
        //FREERAM_PRINT
        PRINT2(F("Free Ram Size: "), mu_freeRam());
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
