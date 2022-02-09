// (Originally from the MemoryUsage library by Thierry Paris)
// modified to return numberic values, and use streams other than Serial

#include "Arduino.h"

#include "MemoryUsageAVR.h"
#include <SimpleSerialShell.h>

#ifdef __AVR__
using namespace MU_AVR;

/// Thanks to adafruit : https://learn.adafruit.com/memories-of-an-arduino/measuring-free-memory

int MU_AVR::maxStackSize = getStackSize();
int MU_AVR::numStackComputeCalls = 0;

// print 1, 2 or 3 items on a line
void PRINT (const __FlashStringHelper *a, int b = -1, const __FlashStringHelper * c = NULL)
{
    shell.print(a);
    if (b != -1) {
        shell.print(b);
        shell.print(F(" (0x"));
        shell.print(b,HEX);
        shell.print(F(")"));
        if (c) {
            shell.print(c);
        }
    }
    shell.println();
}

/// Modified function from http://www.avr-developers.com/mm/memoryusage.html
void MU_AVR::SRamDisplay(void)
{
    int	data_size	=	(int)&__data_end - getDataStart(); // vars initialized on start
    int	bss_size	=	(int)&__bss_end - (int)&__data_end;// vars uninitialized on start
    int heap_end 	= 	getHeapEnd();
    //int	heap_end	=	(int)SP - (int)&__malloc_margin;
    int	heap_size	=	heap_end - (int)&__bss_end;
    int	stack_size	=	getStackSize();
    int	available	=	getRamSize();

    available	-=	data_size + bss_size + heap_size + stack_size;

    PRINT( F( "+----------------+ " ),        getDataStart(),   F(" (__data_start)"));
    PRINT( F( "+      data      +" ));
    PRINT( F( "+    variables   + size = " ), data_size);
    PRINT( F( "+----------------+ " ),        (int)&__data_end, F(" (__data_end / __bss_start)"));
    PRINT( F( "+      bss       +" ));
    PRINT( F( "+    variables   + size = " ), bss_size);
    PRINT( F( "+----------------+ " ),        (int)&__bss_end,  F(" (__bss_end / __heap_start)"));
    PRINT( F( "+      heap      + size = " ), heap_size);
    PRINT( F( "+----------------+ " ),        (int)heap_end,    F(" (__brkval if not 0, or __heap_start)"));
    PRINT( F( "+                +" ));
    PRINT( F( "+                +" ));
    PRINT( F( "+   FREE RAM     + size = " ), available);
    PRINT( F( "+                +" ));
    PRINT( F( "+                +" ));
    PRINT( F( "+----------------+ " ),        (int)SP,           F(" (SP)"));
    PRINT( F( "+     stack      + size = " ), stack_size);
    PRINT( F( "+----------------+ " ),        (int)RAMEND,       F(" (RAMEND / __stack)"));
    shell.println();
    shell.println();
}

#endif
