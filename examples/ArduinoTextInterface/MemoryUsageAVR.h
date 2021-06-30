// (Originally from the MemoryUsage library by Thierry Paris)
// modified to return numberic values, and use streams other than Serial
//
// MemoryUsageAVR.h

#ifdef __AVR__

#ifndef __MemoryUsageAVR_h__
#define __MemoryUsageAVR_h__

#include <stdint.h>
//#include <SimpleSerialShell>

/*! \mainpage

    A full explanation in french can be read at http://www.locoduino.org/ecrire/?exec=article&action=redirect&type=article&id=149 .

    Roughly, the SRAM memory is divided into four areas: the static data, the heap, the free ram and the stack.

    The static data size is given by the compiler itself after the building. this is filled by all variables and
    arrays declared in global scope, or with 'static' keyword.

    The heap is filled with all the dynamic allocations done with 'new' keyword or 'malloc' functions.

    The stack start from the end of the SRAM area and grow and shrink downward at each function call, it stores
    all the local data internal to a function, function arguments (depending of the architecture, arguments can be
    stored in CPU registers to improve speed...) , and addresses for function returns to caller.

    SRAM memory
    \verbatim
    +---------------+------------------+---------------------------------------------+-----------------+
    |               |                  |                                             |                 |
    |               |                  |                                             |                 |
    |    static     |                  |                                             |                 |
    |     data      |       heap       |                   free ram                  |      stack      |
    |               |                  |                                             |                 |
    |               |                  |                                             |                 |
    |               |                  |                                             |                 |
    +---------------+------------------+---------------------------------------------+-----------------+
       _end or __heap_start     __brkval                                         SP             RAMEND
    \endverbatim

    Source : http://www.nongnu.org/avr-libc/user-manual/malloc.html

    MemoryUsage try to help you to find the actual memory status with differents strategies, but dont forget
    that when you observe something, you change the result of the observation : execution time is consumed
    by the analysis tools, and memory used will grow because of these tools !

    1. First, there are the MACROs to show memory areas start/end addresses and actual sizes.

    2. Second, there is a display function to show a 'map' of the memory...

    3. Third, a function can give you the current size of the free ram using a stack tag, which is more accurate
    than the MACRO.

    4. Fourth, an elegant way to try to understand how much size has reached the stack during execution.
    It will 'decorate' the internal memory, and try to identify after a moment of execution at what place
    the first byte of the memory is not anymore decorated...
    The function mu_StackPaint will be called _before the setup() function of your sketch, to 'paint' or
    'decorate' all the bytes of the SRAM momery with a particular code, called the CANARY... Later, a function
    mu_StackCount can be called to get the actual maximum size reached by the stack by counter the byte
    no more painted.
    This is a copy / adaptation of the library StackPaint available here : https://github.com/WickedDevice/StackPaint

    5. And five at least, and because a stack grow and shrink continuously, the macros STACK_DECLARE / STACK_COMPUTE / STACK_PRINT
    try to get the greatest size of the stack by 'sampling' the execution.
    Start your code by

    #include <MemoryUsageAVR.h>

    void setup()
    ...

    then add a STACK_COMPUTE in any function that can be called :

    void subFunction()
    {
        double v[SIZE];
        STACK_COMPUTE;

        .... // do things
    }

    and finish by printing on the console the biggest size of the stack with STACK_PRINT or STACK_PRINT_TEXT.
    Be careful with this method, this introduce  some code in every function of your sketch, so if the timing
    is important for your applicaion, take care of it !
*/

/*! \file MemoryUsage.h

    Main library header file.
*/

extern uint8_t _end;
extern uint8_t __stack;
extern uint8_t *__brkval;
extern uint8_t *__data_start;
extern uint8_t *__data_end;
extern uint8_t *__heap_start;
extern uint8_t *__heap_end;
extern uint8_t *__bss_start;
extern uint8_t *__bss_end;

extern int numStackComputeCalls;
extern int mu_stack_size;

///////////////////////////////////////////////////////////////////////////////////////////////
//
// Memory addresses
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
    return mu_stack_size;
}

inline unsigned int getRamSize(void)
{
    return (getMemoryEnd() + 1) - getDataStart();
}
//}; // end namespace MU_AVR

//#define MU_PRINT(txt, value)
//    {
//        Serial.print((txt));
//        Serial.println((value));
//    }

///// Print data start on serial console.
//#define MEMORY_PRINT_START MU_PRINT(F("Data start:"), MU_AVR::getDataStart())
///// Print data end / heap start on serial console.
//#define MEMORY_PRINT_HEAPSTART MU_PRINT(F("Heap start:"), MU_AVR::getHeapStart())
///// Print heap end / free ram area on serial console.
//#define MEMORY_PRINT_HEAPEND MU_PRINT(F("Heap end:"), MU_AVR::getHeapEnd())
///// Print free ram end / stack start on serial console.
//#define MEMORY_PRINT_STACKSTART MU_PRINT(F("Stack start:"), MU_AVR::getStackStart())
///// Print end of memory on serial console.
//#define MEMORY_PRINT_END MU_PRINT(F("Stack end:"), MU_AVR::getMemoryEnd())
//
///// Print heap size on serial console.
//#define MEMORY_PRINT_HEAPSIZE MU_PRINT(F("Heap size:"), MU_AVR::getHeapSize())
///// Print stack size on serial console.
//#define MEMORY_PRINT_STACKSIZE MU_PRINT(F("Stack size:"), MU_AVR::getStackSize())
///// Print free ram size on serial console.
//#define MEMORY_PRINT_FREERAM MU_PRINT(F("Free ram:"), ; MU_AVR::getFreeRam())
///// Print total SRAM size on serial console.
//#define MEMORY_PRINT_TOTALSIZE MU_PRINT(F("SRAM size:"), MU_AVR::getRamSize())

/// Displays the 'map' of the current state of the Arduino's SRAM memory on the Serial console.
void SRamDisplay(void);

//
// Stack count part. STACK_COMPUTE will get the maximum size of the stack at the moment...
//

/// Must be used only one time, outside any function.
//#define STACK_DECLARE int mu_stack_size = ((int)RAMEND - (int)SP);

/// Must be called to update the current maximum size of the stack, at each function beginning.
//#define STACK_COMPUTE      { mu_stack_size = (RAMEND - SP) > mu_stack_size ? (RAMEND - SP) : mu_stack_size;}
inline void stackCompute(void)
{
    //int currentStack = (int)SP;
    //int currentSize = (int)RAMEND - currentStack;
    int currentSize = getStackSize();
    mu_stack_size = max(currentSize, mu_stack_size);
    numStackComputeCalls++;
    //Serial.println(F("COMPUTING STACK..."));
}
#define STACK_COMPUTE stackCompute();

///// Compute the current maximum and show it now with customized text.
//#define STACK_PRINT_TEXT(text)
//    {
//        STACK_COMPUTE;
//        Serial.print(text);
//        Serial.println(mu_stack_size);
//    }

/// Compute the current maximum and show it now with default text.
//#define STACK_PRINT STACK_PRINT_TEXT(F("Stack Maximum Size (Instrumentation method): "));

//
// Free Ram part.
//

///// Shows the current free SRAM memory with customized text.
//#define FREERAM_PRINT_TEXT(text)
//    Serial.print(text);
//    Serial.println(mu_freeRam());
//
///// Shows the current free SRAM memory with default text.
//#define FREERAM_PRINT FREERAM_PRINT_TEXT(F("Free Ram Size: "));

/// return the free Ram size on console.
int mu_freeRam(void);


/// return max stack size on console.
uint16_t mu_StackCount(void);

}; // end namespace MU_AVR

#endif
#else // __AVR__
#define STACK_COMPUTE /* do nothing; MemoryUsageAVR library emulation not included */
#endif
