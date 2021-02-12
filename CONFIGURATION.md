# SimpleSerialShell Configuration
## Input buffer size
The predefined size of input buffer is 88 bytes (defined by the ``SIMPLE_SERIAL_SHELL_BUFSIZE`` identifier). It's ok in most cases, but when it's not enough, you can redefine it with build options.
Check your IDE documentation to learn how to properly redefine build options. See the example for PlatformIO below.

### Arduino IDE:
Configuring buffer size from within the Arduino IDE is not recommended.

### PlatformIO:
Define a new value at your `platformio.ini` in this way:
```ini
build_flags = -D SIMPLE_SERIAL_SHELL_BUFSIZE=128
```

An example for Leonardo board looks this way:
```ini
leonardo.build.extra_flags={build.usb_flags} '-DSIMPLE_SERIAL_SHELL_BUFSIZE=128'
```
