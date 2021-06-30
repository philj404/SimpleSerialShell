// (Originally from the MemoryUsage library by Thierry Paris)
// modified to return numberic values, and use streams other than Serial

#include "Arduino.h"

#include "MemoryUsageAVR.h"
#include <SimpleSerialShell.h>

#ifdef __AVR__
using namespace MU_AVR;

/// Thanks to adafruit : https://learn.adafruit.com/memories-of-an-arduino/measuring-free-memory
int MU_AVR::mu_freeRam()
{
    return getFreeRam();
}

int mu_stack_size = getStackSize();
int numStackComputeCalls = 0;

// print 1, 2 or 3 items on a line
#define PRINT1(a) shell.println((a));
#define PRINT2(a,b) shell.print((a)); shell.println((b));
#define PRINT3(a,b,c) shell.print((a)); shell.print((b)); shell.println((c));

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

    PRINT3( F( "+----------------+ " ),        getDataStart(),   F(" (__data_start)"));
    PRINT1( F( "+      data      +" ));
    PRINT2( F( "+    variables   + size = " ), data_size);
    PRINT3( F( "+----------------+ " ),        (int)&__data_end, F(" (__data_end / __bss_start)"));
    PRINT1( F( "+      bss       +" ));
    PRINT2( F( "+    variables   + size = " ), bss_size);
    PRINT3( F( "+----------------+ " ),        (int)&__bss_end,  F(" (__bss_end / __heap_start)"));
    PRINT2( F( "+      heap      + size = " ), heap_size);
    PRINT3( F( "+----------------+ " ),        (int)heap_end,    F(" (__brkval if not 0, or __heap_start)"));
    PRINT1( F( "+                +" ));
    PRINT1( F( "+                +" ));
    PRINT2( F( "+   FREE RAM     + size = " ), available);
    PRINT1( F( "+                +" ));
    PRINT1( F( "+                +" ));
    PRINT3( F( "+----------------+ " ),        (int)SP,           F(" (SP)"));
    PRINT2( F( "+     stack      + size = " ), stack_size);
    PRINT3( F( "+----------------+ " ),        (int)RAMEND,       F(" (RAMEND / __stack)"));
    shell.println();
    shell.println();
}

#endif
