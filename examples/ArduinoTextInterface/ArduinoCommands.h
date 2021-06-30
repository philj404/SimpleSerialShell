// An external reference to the Arduino command wrappers.
//
#include <SimpleSerialShell.h>

extern int addArduinoCommands(SimpleSerialShell &shell);
extern int addMemoryCommands(SimpleSerialShell &shell);

#ifdef AVR
extern int addStackHeapCommands(SimpleSerialShell &shell);
extern int addMemoryUsageCommands(SimpleSerialShell &shell);
#else
//#define STACK_COMPUTE /* do nothing; MemoryUsageAVR library emulation not included */
#endif
