# SimpleSerialShell
An extensible command shell for Arduino.

This library provides a basic "command line interface" for your Arduino through its serial interface.  
This can connect to the Serial Monitor, for example.

Commands are of the old (1970s) Unix-style " **int aCommand(int argc, char ** argv)** " described in Kernighan and Ritchie.

It's just a simple shell... enough that the Serial Monitor can send text commands and receive text responses.  OK for humans with keyboards.

Note 2:  The intent is to be _human-readable_ rather than _fast_.  
If you want fast serial communication between an Arduino and a host, 
for example, you may want to look into Wiring or MIDI protocols (which I don't really know about).

Example:

    #include <SimpleSerialShell.h>

    int helloWorld(int argc, char **argv) {...};

    setup()
    {
      Serial.begin(9600);
   
      shell.attach(Serial);  // attach to any stream...
      shell.addCommand(F("sayHello"),helloWorld);
    }
    loop()
    {
      shell.executeIfInput();
    }

Example Sketches
* AdjustableBlink -- lets you read and vary the blink rate of a Blink sketch.
* ArduinoTextInterface
lets you read and write digital and analog values to Arduino pins.  Basically wrappers for analogRead(), analogWrite(), tone(), noTone() etc.
It's enough to set and clear bits one at a time and see if you have an LED connected to the right pin.
* EchoCommand -- "echo" example.  
Sending "echo Hello World!" gives "Hello World!" on the serial monitor.
* IdentifyTheSketch -- Example provides an "id?" query which reports the filename and build date of the sketch running.  
Useful if you forgot what was loaded on this board.
