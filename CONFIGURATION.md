# SimpleSerialShell Configuration
## Input buffer size
The predefined size of input buffer is 88 bytes (defined by the ``SIMPLE_SERIAL_SHELL_BUFSIZE`` identificator). It's ok in most cases, but when it's not enough, you can redefine it with build options.
Check your IDE documentation to learn how to properly redefine build options. See examples for Arduino IDE and PlatformIO below.

### Arduino IDE:
Now Arduino IDE allows to redefine build options through definition of board parameters.
Find the `*board_name*.build.extra_flags` parameter for your board at  `*path_to_arduino_ide*/hardware/arduino/avr/boards.txt` file and set a new limit:
`-DSIMPLE_SERIAL_SHELL_BUFSIZE=128`.

### PlatformIO:
Define a new value at your `platformio.ini` in this way:
```ini
build_flags = -D SIMPLE_SERIAL_SHELL_BUFSIZE=128
```

An example for Leonardo board looks this way:
```ini
leonardo.build.extra_flags={build.usb_flags} '-DSIMPLE_SERIAL_SHELL_BUFSIZE=128'
```