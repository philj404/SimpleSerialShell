# SimpleSerialShell # [![Build Status](https://travis-ci.com/philj404/SimpleSerialShell.svg?branch=main)](https://travis-ci.com/philj404/SimpleSerialShell)
An extensible command shell for Arduino.

This library provides a basic "command line interface" for your Arduino through its serial interface.  
This can connect to the Serial Monitor, for example.

Commands are of the old (1970s) Unix-style
` int aCommand(int argc, char ** argv) ` as described in Kernighan and Ritchie.

### Example:
Include the library:
```cpp
#include <SimpleSerialShell.h>
```

Create the command to call:
```cpp
int helloWorld(int argc, char **argv) {...};

```

Start the serial connection and attach it to the shell:

```cpp
setup()
{
  Serial.begin(9600);

  shell.attach(Serial);  // attach to any stream...
  ...
}
 ```
Name the command and add it to the shell:
```cpp
  ...
  shell.addCommand(F("sayHello"),helloWorld);
  ...
```

Check for input periodically:
```cpp
loop()
{
  ...
  shell.executeIfInput();
  ...
}
 ```

### Example Sketches
* **AdjustableBlink**
-- lets you read and vary the blink rate of a Blink sketch.
* **AlternateTokenizer**
-- Demonstrates an echo command that accepts quoted tokens that contain spaces.
* **ArduinoTextInterface**
-- lets you read and write digital and analog values to Arduino pins.  Basically wrappers for setPinMode(), digitalRead(), digitalWrite(), analogRead(), analogWrite(), etc.
It's incomplete, but enough to set and clear bits one at a time and see if you have an LED connected to the right pin.
* **EchoCommand** -- "echo" example.  
Sending "echo Hello World!" returns "Hello World!" on the serial monitor.
* **IdentifyTheSketch** -- Example provides an "id?" query which reports the filename and build date of the sketch running.  
Useful if you forgot what was loaded on this board.

### Alternate Tokenizers

By default, shell input is tokenized using the UNIX standard strtok_r(3) function.  This splits user input into space-delimited tokens. There may be applications where a more sophisticated tokenizer is desired. Quoted tokens with internal spaces, for example.  The setTokenizer() method can be used to install a custom tokenizer.

A demonstration of this feature can be seen [here](examples/AlternateTokenizer).

### Tips

* "help" is a built-in command.  It lists what is available.

* If memory limitations allow, provide additional documentation for each command you register following a colon delimeter in the addCommand() method.  This will make the "help" output more comprehensive and
may make your device more user-friendly.  (For example, use `shell.addCommand(F("echo <wordsToEcho>..."), echoCommand);` )

* RAM is limited in the ATMega world.  To save space, use the F() macro, which keeps const strings in flash
rather than copying them to RAM.  (For example use `shell.addCommand(F("sayHello"), helloWorld);` )

* Since the shell delegates the actual communication to what it connects to
(with shell.attach()), it can work with Serial, Serial2, SoftwareSerial or
a custom stream.

* To make it easy to switch commands to a different connection, I recommend always
sending command output to the shell
(rather than straight to Serial for example).  For example I use `shell.println("motor is off");`

### Notes

It's just a simple shell... enough that the Serial Monitor can send text
commands and receive text responses.  It's OK for humans with keyboards.

The intent is for the Serial Monitor output to be _human-readable_ rather than _fast_.  
If you want fast serial DATA transfer between an Arduino and a host, 
 you may want to look into Processing/Wiring, MIDI or other protocols
(which I don't really know about).

Each added command uses up a small amount of limited RAM.  This may run out
if you need MANY commands.  (Less of a problem for newer processors.)


(I haven't tested this but) you should be able to switch among multiple
streams on the fly.  Not sure if that's useful with limited code space.

