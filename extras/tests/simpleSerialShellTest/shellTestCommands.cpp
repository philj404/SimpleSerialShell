// shellTestCommands.cpp
//
#include <Arduino.h>
#include <SimpleSerialShell.h>
#include "shellTestHelpers.h"

////////////////////////////////////////////////////////////////////////////////
// echo
// simple version of Unix style 'echo'
//
// example:  "echo Hello world"
//  prints:  "Hello world\n" to attached stream (Serial, Serial1 etc.).
//
////////////////////////////////////////////////////////////////////////////////
//
int echo(int argc, char **argv)
{
    auto lastArg = argc - 1;
    for ( int i = 1; i < argc; i++) {

        shell.print(argv[i]);

        if (i < lastArg)
            shell.print(F(" "));
    }
    shell.println();

    return EXIT_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////
// calculate the sum of the arguments
// "sum 5 3" returns 8.
//
int sum(int argc, char **argv)
{
    int aSum = 0;

    for ( int i = 1; i < argc; i++) {
        aSum += atoi(argv[i]);
    }

    return aSum;
}


void addTestCommands(void) {
    shell.addCommand(F("echo"), echo);
    shell.addCommand(F("sum"), sum);
}
