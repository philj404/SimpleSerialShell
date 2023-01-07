Release notes

### v1.0.0
* save another ~8 bytes RAM for ATmega processors
* declare a stable 1.0 release

### v0.9.2
* #28 bug - use full command name (no partial matches)
* added example memory dump commands

### v0.9.1
* Ensure shell is a Singleton

### v0.9.0
* Support argument hints when adding commands (for "help" command) (brucemack)
* Add "> " command prompt to show shell is alive (MAY BREAK TESTS of text output)
* Unit tests build/run/PASS with Windows/Arduino IDE + Arduino Nano

### v0.2.3
* Support custom tokenizers, for example to quote text with spaces. (brucemack)
* Clean up miscellaneous warnings

### v0.2.2
examples/ArduinoTextInterface:
* Provide memory dump commands for RAM, ROM and EEPROM address spaces.  Very useful for finding/removing constant strings in RAM.
* Provide memory usage info for RAM consumption (AVR architecture).  Shows global, heap, stack and free memory measurements.

### v0.2.1
(for PlatformIO builds) Support changing the default buffer size with a #define. (eg321)

### v0.2.0
Run unit tests and confirm they PASS on github code push.
More constistent code formatting (with astyle)

### v0.1.1
Confirm library can build for various platforms on github code push.
(suggested by KiraVerSace)

### v0.1.0
Initial release
 
