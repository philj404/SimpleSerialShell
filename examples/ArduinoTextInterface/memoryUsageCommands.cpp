#include <Arduino.h>
#include "ArduinoCommands.h"

#if defined(WITH_MEMORY_USAGE)

#include <MemoryUsage.h>

// declare exactly once
STACK_DECLARE

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
  { // default
    graphical = true;
  }
  else
  {
    for (char *flag = argv[1]; *flag != 0; flag++)
    {
      switch (*flag)
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
  
  if(all)
  {
    MEMORY_PRINT_START
  }
  if (heap || all)
  {
    MEMORY_PRINT_HEAPSTART
    MEMORY_PRINT_HEAPEND
    MEMORY_PRINT_HEAPSIZE
  }
  if (stack || all)
  {
    MEMORY_PRINT_STACKSTART
    MEMORY_PRINT_END
    MEMORY_PRINT_STACKSIZE
    STACK_PRINT
    //STACKPAINT_PRINT  // I'm not sure I trust paint initialization
  }
  if (free || all)
  {
    FREERAM_PRINT
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
