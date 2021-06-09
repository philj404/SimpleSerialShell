// An external reference to the Arduino command wrappers.
//
#include <SimpleSerialShell.h>

extern int addArduinoCommands(SimpleSerialShell &shell);
extern int addMemoryCommands(SimpleSerialShell &shell);
extern int addStackHeapCommands(SimpleSerialShell &shell);

//#define USE_COMMAND_TABLE

// OPTIONAL: requires the MemoryUsage library
#define WITH_MEMORY_USAGE
#if defined(WITH_MEMORY_USAGE)
extern int addMemoryUsageCommands(SimpleSerialShell &shell);
#else
#define STACK_COMPUTE /* do nothing; MemoryUsage library not included */
#endif
