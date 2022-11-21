#include <Arduino.h>
#include <SimpleSerialShell.h>

////////////////////////////////////////////////////////////////////////////////
/*!
 *  @file SimpleSerialShell.cpp
 *
 *  Implementation for the shell.
 *
 */

// The static instance of the singleton
SimpleSerialShell SimpleSerialShell::theShell;

// A reference to the singleton shell in the global namespace. There is an
// extern definition of this in SimpleSherialShell.h, so all users of the
// class will have visibilty to this reference.
SimpleSerialShell& shell = SimpleSerialShell::theShell;

//
SimpleSerialShell::Command * SimpleSerialShell::firstCommand = NULL;

////////////////////////////////////////////////////////////////////////////////
/*!
 *  @brief associates a named command with the function to call.
 */
class SimpleSerialShell::Command {
    public:
        Command(const __FlashStringHelper * n, CommandFunction f):
            nameAndDocs(n), myFunc(f) {};

        int execute(int argc, char **argv)
        {
            return myFunc(argc, argv);
        };

        // Comparison used for sort commands
        int compare(const Command * other) const
        {
            const String otherNameString(other->nameAndDocs);
            return compareName(otherNameString.c_str());
        };

        int compareName(const char * aName) const
        {
            // Look for the command delimiter and make sure we don't
            // consider anything beyond it in the comparison.  There
            // may be more documentation in the string.
            //
            // Note for future consideration: The temporary String here could
            // be eliminated here by leveraging strlen_P, pgm_read_byte, and
            // strncasecmp_P.  That will take a bit of research since
            // the header file may have a different name on ESP2886/ESP32.
            String work(nameAndDocs);
            int delim = work.indexOf(' ');
            if (delim >= 0) {
                work.remove(delim);
            }
            return strncasecmp(work.c_str(), aName, SIMPLE_SERIAL_SHELL_BUFSIZE);
        };

        /**
         * @brief Writes the documentation associated with this command.
         *
         * @param str Stream to write into.
         */
        void renderDocumentation(Stream& str) const
        {
            str.print(F("  "));
            str.print(nameAndDocs);
            str.println();
        }

        Command * next;

    private:

        const __FlashStringHelper * const nameAndDocs;
        const CommandFunction myFunc;
};

////////////////////////////////////////////////////////////////////////////////
SimpleSerialShell::SimpleSerialShell()
    : shellConnection(NULL),
      m_lastErrNo(EXIT_SUCCESS),
      tokenizer(strtok_r)
{
    resetBuffer();

    // simple help.
    addCommand(F("help"), SimpleSerialShell::printHelp);
};

//////////////////////////////////////////////////////////////////////////////
void SimpleSerialShell::addCommand(
    const __FlashStringHelper * name, CommandFunction f)
{
    auto * newCmd = new Command(name, f);

    // insert in list alphabetically
    // from stackoverflow...

    Command* temp2 = firstCommand;
    Command** temp3 = &firstCommand;
    while (temp2 != NULL && (newCmd->compare(temp2) > 0) )
    {
        temp3 = &temp2->next;
        temp2 = temp2->next;
    }
    *temp3 = newCmd;
    newCmd->next = temp2;
}

//////////////////////////////////////////////////////////////////////////////
bool SimpleSerialShell::executeIfInput(void)
{
    bool bufferReady = prepInput();
    bool didSomething = false;

    if (bufferReady) {
        didSomething = true;
        execute();
	print(F("> ")); // provide command prompt feedback
    }

    return didSomething;
}

//////////////////////////////////////////////////////////////////////////////
void SimpleSerialShell::attach(Stream & requester)
{
    shellConnection = &requester;
}

//////////////////////////////////////////////////////////////////////////////
// Arduino serial monitor appears to 'cook' lines before sending them
// to output, so some of this is overkill.
//
// But for serial terminals, backspace would be useful.
//
bool SimpleSerialShell::prepInput(void)
{
    bool bufferReady = false; // assume not ready
    bool moreData = true;

    do {
        int c = read();
        switch (c)
        {
            case -1: // No character present; don't do anything.
                moreData = false;
                break;
            case  0: // throw away NUL characters
                break;

            // Line editing characters
            case 127: // DEL delete key
            case '\b':  // CTRL(H) backspace
                // Destructive backspace: remove last character
                if (inptr > 0) {
                    print(F("\b \b"));  // remove char in raw UI
                    linebuffer[--inptr] = 0;
                }
                break;

            case 0x12: //CTRL('R')
                //Ctrl-R retypes the line
                print(F("\r\n"));
                print(linebuffer);
                break;

            case 0x15: //CTRL('U')
                //Ctrl-U deletes the entire line and starts over.
                println(F("XXX"));
                resetBuffer();
                break;

            case ';':   // BLE monitor apps don't let you add '\r' to a string,
            // so ';' ends a command

            case '\r':  //CTRL('M') carriage return (or "Enter" key)
                // raw input only sends "return" for the keypress
                // line is complete
                println();     // Echo newline too.
                bufferReady = true;
                break;

            case '\n':  //CTRL('J') linefeed
                // ignore newline as 'raw' terminals may not send it.
                // Serial Monitor sends a "\r\n" pair by default
                break;

            default:
                // Otherwise, echo the character and append it to the buffer
                linebuffer[inptr++] = c;
                write(c);
                if (inptr >= SIMPLE_SERIAL_SHELL_BUFSIZE-1) {
                    bufferReady = true; // flush to avoid overflow
                }
                break;
        }
    } while (moreData && !bufferReady);

    return bufferReady;
}

//////////////////////////////////////////////////////////////////////////////
int SimpleSerialShell::execute(const char commandString[])
{
    // overwrites anything in linebuffer; hope you don't need it!
    strncpy(linebuffer, commandString, SIMPLE_SERIAL_SHELL_BUFSIZE);
    return execute();
}

//////////////////////////////////////////////////////////////////////////////
int SimpleSerialShell::execute(void)
{
    char * argv[MAXARGS] = {0};
    linebuffer[SIMPLE_SERIAL_SHELL_BUFSIZE - 1] = '\0'; // play it safe
    int argc = 0;

    char * rest = NULL;
    const char * whitespace = " \t\r\n"; // not PROGMEM/simple TokenizerFunction
    char * commandName = tokenizer(linebuffer, whitespace, &rest);

    if (!commandName)
    {
        // empty line; no arguments found.
        println(F("OK"));
        resetBuffer();
        return EXIT_SUCCESS;
    }
    argv[argc++] = commandName;

    for ( ; argc < MAXARGS; )
    {
        char * anArg = tokenizer(0, whitespace, &rest);
        if (anArg) {
            argv[argc++] = anArg;
        } else {
            // no more arguments
            return execute(argc, argv);
        }
    }

    return report(F("Too many arguments to parse"), -1);
}

//////////////////////////////////////////////////////////////////////////////
int SimpleSerialShell::execute(int argc, char **argv)
{
    m_lastErrNo = 0;
    for ( Command * aCmd = firstCommand; aCmd != NULL; aCmd = aCmd->next) {
        if (aCmd->compareName(argv[0]) == 0) {
            m_lastErrNo = aCmd->execute(argc, argv);
            resetBuffer();
            return m_lastErrNo;
        }
    }
    print(F("\""));
    print(argv[0]);
    print(F("\": "));

    return report(F("command not found"), -1);
}

//////////////////////////////////////////////////////////////////////////////
int SimpleSerialShell::lastErrNo(void)
{
    return m_lastErrNo;
}

//////////////////////////////////////////////////////////////////////////////
int SimpleSerialShell::report(const __FlashStringHelper * constMsg, int errorCode)
{
    if (errorCode != EXIT_SUCCESS)
    {
        String message(constMsg);
        print(errorCode);
        if (message[0] != '\0') {
            print(F(": "));
            println(message);
        }
    }
    resetBuffer();
    m_lastErrNo = errorCode;
    return errorCode;
}
//////////////////////////////////////////////////////////////////////////////
void SimpleSerialShell::resetBuffer(void)
{
    memset(linebuffer, 0, sizeof(linebuffer));
    inptr = 0;
}

//////////////////////////////////////////////////////////////////////////////
// SimpleSerialShell::printHelp() is a static method.
// printHelp() can access the linked list of commands.
//
int SimpleSerialShell::printHelp(int /*argc*/, char ** /*argv*/)
{
    shell.println(F("Commands available are:"));
    auto aCmd = firstCommand;  // first in list of commands.
    while (aCmd)
    {
        aCmd->renderDocumentation(shell);
        aCmd = aCmd->next;
    }
    return 0;	// OK or "no errors"
}

///////////////////////////////////////////////////////////////
// i/o stream indirection/delegation
//
size_t SimpleSerialShell::write(uint8_t aByte)
{
    return shellConnection ?
           shellConnection->write(aByte)
           : 0;
}

int SimpleSerialShell::available()
{
    return shellConnection ? shellConnection->available() : 0;
}

int SimpleSerialShell::read()
{
    return shellConnection ? shellConnection->read() : 0;
}

int SimpleSerialShell::peek()
{
    return shellConnection ? shellConnection->peek() : 0;
}

void SimpleSerialShell::flush()
{
    if (shellConnection)
        shellConnection->flush();
}

void SimpleSerialShell::setTokenizer(TokenizerFunction f)
{
    tokenizer = f;
}
