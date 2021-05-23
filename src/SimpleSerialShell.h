
#ifndef SIMPLE_SERIAL_SHELL_H
#define SIMPLE_SERIAL_SHELL_H

#ifndef SIMPLE_SERIAL_SHELL_BUFSIZE
#define SIMPLE_SERIAL_SHELL_BUFSIZE 88
#endif

////////////////////////////////////////////////////////////////////////////////
/*!
 *  @file SimpleSerialShell.h
 *
 *  @section dependencies Dependencies
 *
 *  Depends on Stream.  The shell is an instance of Stream so anthing that
 *  works with a Stream should also work with the shell.
 *
 *  @section author Phil Jansen
 */
class SimpleSerialShell : public Stream
{
public:
    SimpleSerialShell(void);

    // Unix-style (from 1970!)
    // functions must have a signature like:
    //
    //     int hello(int argc, char ** argv)
    //
    typedef int (*CommandFunction)(int, char **);

    // add commands independently
    //void addCommand(const char * name, CommandFunction f);
    void addCommand(const __FlashStringHelper *name, CommandFunction f);

    // add commands as one constant table/array
    //struct CommandEntry {const __FlashStringHelper * name; CommandFunction f;};
    class CommandEntry
    {
    public:
        //CommandEntry(const __FlashStringHelper name, const CommandFunction f);
        CommandEntry(const char *PROGMEM _name, const CommandFunction _f)
            : name(_name), myFunc(_f){};
        int compareName(const char *aName) const;
        int execute(int argc, char **argv) const;

        //private:
        const char *PROGMEM name;
        //const char * name;
        //const __FlashStringHelper * name;
        const CommandFunction myFunc;
    };

    void addCommandTable(const CommandEntry aTable[], int byteSizeOfTable);

    void attach(Stream &shellSource);

    // check for a complete command and run it if available
    // non blocking
    bool executeIfInput(void); // returns true when command attempted
    int lastErrNo(void);

    int execute(const char aCommandString[]); // shell.execute("echo hello world");

    static int printHelp(int argc, char **argv);

    void resetBuffer(void);

    // this shell delegates communication to/from the attached stream
    // (which sent the command)
    // Note changing streams may intermix serial data
    //
    virtual size_t write(uint8_t);
    virtual int available();
    virtual int read();
    virtual int peek();
    virtual void flush(); // esp32 needs an implementation

private:
    Stream *shellConnection;
    int m_lastErrNo;
    int execute(void);
    int execute(int argc, char **argv);

    bool prepInput(void);

    int report(const __FlashStringHelper *message, int errorCode);
    static const char MAXARGS = 10;
    char linebuffer[SIMPLE_SERIAL_SHELL_BUFSIZE];
    int inptr;

    class Command;
    static Command *firstCommand;
    static const CommandEntry *commandTable;
    static int numTableEntries;
};

////////////////////////////////////////////////////////////////////////////////
extern SimpleSerialShell shell;

//example commands which would be easy to add to the shell:
//extern int helloWorld(int argc, char **argv);
//extern int echo(int argc, char **argv);

#endif /* SIMPLE_SERIAL_SHELL_H */
