// memory_probe.cpp
//
// Storage measurement
//
// See also:
// https://playground.arduino.cc/Code/AvailableMemory/
// https://learn.adafruit.com/memories-of-an-arduino/measuring-free-memory
// https://github.com/mpflaga/Arduino-MemoryFree
// https://github.com/Locoduino/MemoryUsage
#ifdef AVR
#include <Arduino.h>
#include "ArduinoCommands.h"

#ifdef __arm__
// should use uinstd.h to define sbrk but Due causes a conflict
extern "C" char* sbrk(int incr);
#else  // __ARM__
//extern char *__brkval;
extern uint8_t *__brkval;
#endif  // __arm__

//extern unsigned int __heap_start;
extern uint8_t * __heap_start;
//extern void *__brkval;

/*
    The free list structure as maintained by the
    avr-libc memory allocation routines.

    Warning: this __freelist struct matches the real one ONLY BY COINCIDENCE
    Keep it in sync!
*/
struct __freelist {
    size_t sz;
    struct __freelist *nx;
};

/* The head of the free list structure */
extern struct __freelist *__flp;

//#include "MemoryFree.h"

/* Calculates the size of the free list */
// may be fragmented/unusable...
int freeListSize() {
    struct __freelist* current;
    int total = 0;
    int numFragments = 0;
    for (current = __flp; current; current = current->nx) {
        total += 2; /* Add two bytes for the memory block's header  */
        total += (int) current->sz;
        numFragments++;
    }
    if (numFragments) {
        Serial.print(F("unrecovered fragments: "));
        Serial.println(numFragments);
    }
    if (total) {
        Serial.print(F("(fragmented) free list space: "));
        Serial.println(total);
    }
    return total;
}

int memorySimpleProbe(int, char **)
{
    int free_memory;  // also at top of stack
    if ((int)__brkval == 0) {
        shell.println(F("no malloc()/heap in use yet."));
        free_memory = ((int)&free_memory) - ((int)&__heap_start);
    } else {
        //Serial.println(F("including size of free list."));
        free_memory = ((int)&free_memory) - ((int)__brkval);
        shell.print(F("Unfragmented memory (usable for stack): "));
        shell.println(free_memory);
        //Serial.print(F("free list space: "));
        //Serial.println(freeListSize());
        free_memory += freeListSize();
    }
    shell.print(F("Total free memory "));
    shell.println(free_memory);

    return free_memory;
}

int addStackHeapCommands(SimpleSerialShell &shell)
{
    shell.addCommand(F("stackheap?"), memorySimpleProbe);
    return 0;
}

#endif
