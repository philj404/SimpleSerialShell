// (Originally from the MemoryUsage library by Thierry Paris
// (see https://github.com/Locoduino/MemoryUsage)
// modified to return numberic values, and use streams other than Serial
//
// MemoryUsageAVR.h

#ifdef __AVR__

#ifndef __MemoryUsageAVR_h__
#define __MemoryUsageAVR_h__

#include <stdint.h>

extern uint8_t _end;
extern uint8_t __stack;
extern uint8_t *__brkval;
extern uint8_t *__data_start;
extern uint8_t *__data_end;
extern uint8_t *__heap_start;
extern uint8_t *__heap_end;
extern uint8_t *__bss_start;
extern uint8_t *__bss_end;

///////////////////////////////////////////////////////////////////////////////////////////////
//
// Access Memory addresses
// These are inline functions to minimize how much they affect the stack.
//
namespace MU_AVR
{
// start of RAM:
// ... initialized data
// ... then bss (uninitialized data)
// ... then heap
// ... then FREE RAM
// ... then stack
// ... then end of RAM

inline unsigned int getDataStart(void)
{
    return (unsigned int)&__data_start; // start RAM
}

inline unsigned int getHeapStart(void)
{
    return (unsigned int)&__heap_start;
}

inline unsigned int getHeapEnd(void)
{
    unsigned int heapEnd = getHeapStart();
    if (__brkval)
        heapEnd = (unsigned int)__brkval;
    return heapEnd;
}

inline unsigned int getHeapSize(void)
{
    return getHeapEnd() - getHeapStart();
}

extern int numStackComputeCalls;
extern int maxStackSize;

inline unsigned int getStackStart(void)
{
    // next UNUSED entry
    // active stack above here. Stack grows DOWN from RAMEND
    return (unsigned int)SP;
}

inline unsigned int getFreeRam(void)
{
    return getStackStart() - getHeapEnd();
}

inline unsigned int getMemoryEnd(void)
{
    return (unsigned int)RAMEND; // last byte! (not the byte after)
}

inline unsigned int getStackSize(void)
{
    return getMemoryEnd() - getStackStart();
}

inline unsigned int getMaxStackSize(void)
{
    return maxStackSize;
}

inline unsigned int getRamSize(void)
{
    return (getMemoryEnd() + 1) - getDataStart();
}

/// Displays the 'map' of the current state of the Arduino's SRAM memory on the console.
void SRamDisplay(void);

/// Must be called to update the current maximum size of the stack, at each function beginning.
inline void stackCompute(void)
{
    int currentSize = getStackSize();
    maxStackSize = max(currentSize, maxStackSize);
    numStackComputeCalls++;
    //Serial.println(F("COMPUTING STACK..."));
}
#define STACK_COMPUTE stackCompute();

}; // end namespace MU_AVR

#endif
#else // __AVR__
#define STACK_COMPUTE /* do nothing; MemoryUsageAVR library emulation not included */
#endif
