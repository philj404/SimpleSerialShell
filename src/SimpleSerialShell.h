////////////////////////////////////////////////////////////////////////////////
/*!
 *  @file SimpleSerialShell.h
 *
 *  @section author
 *  Phil Jansen
 */

#ifndef SIMPLE_SERIAL_SHELL_H
#define SIMPLE_SERIAL_SHELL_H

class SimpleSerialShell : public Stream {
  public:
    SimpleSerialShell(void);

    /*!
     * @brief expected function signature for console commands.
     *  Unix-style (from 1970!).
     *
     * Functions must have a signature like: "int hello(int argc, char ** argv)"
     */
    typedef int (*CommandFunction)(int, char ** );

    //void addCommand(const char * name, CommandFunction f);
    void addCommand(const __FlashStringHelper * name, CommandFunction f);

    void attach(Stream & shellSource);

    bool executeIfInput(void);  // returns true when command attempted
    //int lastErrNo(void);

    int execute( const char aCommandString[]);  // shell.execute("echo hello world");

    static int printHelp(int argc, char **argv);

    void resetBuffer(void);

    virtual size_t write(uint8_t);
    virtual int available();
    virtual int read();
    virtual int peek();
    virtual void flush(); // esp32 needs an implementation

  private:
    Stream * shellConnection;	// typically Serial
    int m_lastErrNo;
    int execute(void);
    int execute(int argc, char** argv);

    bool prepInput(void);

    int report(const __FlashStringHelper * message, int errorCode);
    static const char BUFSIZE = 88;
    static const char MAXARGS = 10;
    char linebuffer[BUFSIZE];
    int inptr;

    class Command;
    static Command * firstCommand;
};

////////////////////////////////////////////////////////////////////////////////
extern SimpleSerialShell shell; //! singleton object

//example commands which would be easy to add to the shell:
//extern int helloWorld(int argc, char **argv);
//extern int echo(int argc, char **argv);

#endif /* SIMPLE_SERIAL_SHELL_H */
