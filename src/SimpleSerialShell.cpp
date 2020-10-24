////////////////////////////////////////////////////////////////////////////////
/*!
 *  @file SimpleSerialShell.cpp
 *
 *  @mainpage Simple Serial Shell library
 *
 *  For usage see the README.md description
 *  [https://github.com/philj404/SimpleSerialShell/blob/main/README.md]
 *
 *  @section dependencies Dependencies
 *
 *  Depends on the Arduino environment and its Stream interface.
 *  The shell is an instance of Stream so anything that works with
 *  Stream should also work with the shell.
 *
 *  @section author Author
 *
 *  Phil Jansen
 *
 */

/*
 *  Implementation for the shell.
 *
 */

#include <Arduino.h>
#include <SimpleSerialShell.h>

SimpleSerialShell shell; // singleton

//
SimpleSerialShell::Command * SimpleSerialShell::firstCommand = NULL;

////////////////////////////////////////////////////////////////////////////////
/*!
 *  @brief associates a named command with the function to call.
 *  @param n name of command
 *  @param f CommandFunction to execute when name is found
 */
class SimpleSerialShell::Command {
  public:
    /*!
     * @brief constructor
     * @param n command name
     * @param f function to call
     */
    Command(const __FlashStringHelper * n,
		    CommandFunction f): name(n), myFunc(f) {};

    /*!
     *  @brief execute this command
     *  @param argc number of arguments
     *  @param argv array of argument strings
     *  @return function exit code
     */
    int execute(int argc, char **argv)
    {
      return myFunc(argc, argv);
    };

    /*! @brief used to sort commands
     *  @param other other Command to compare
     *  @return -1, 0, 1 depending on which command should be first
     */
    int compare(const Command * other) const
    {
      const String otherNameString(other->name);
      return compareName(otherNameString.c_str());
    };

    /*!
     * @brief for sorting: is this name before or after the other name?
     *
     * @return 0 if aName matches this command
     *         
     * @param aName the command name
     */
    int compareName(const char * aName) const
    {
      const String myNameString(name);
      int comparison = strncasecmp(myNameString.c_str(), aName, BUFSIZE);
      return comparison;
    };

    const __FlashStringHelper * name; ///< command name
    CommandFunction myFunc;	///< command function
    Command * next;	///< linked list
};

////////////////////////////////////////////////////////////////////////////////
//! @brief constructor
SimpleSerialShell::SimpleSerialShell()
  : shellConnection(NULL),
    m_lastErrNo(EXIT_SUCCESS)
{
  resetBuffer();

  // simple help.
  addCommand(F("help"), SimpleSerialShell::printHelp);
};

//////////////////////////////////////////////////////////////////////////////
/*!
 * @brief add a command for the shell to recognize
 * @param name command name for example F("echo")
 * @param f function to call when name is recognized
 */
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
/*! @brief collect input and execute command if line complete
 *  @return true if a command was executed
 */
bool SimpleSerialShell::executeIfInput(void)
{
  bool bufferReady = prepInput();
  bool didSomething = false;

  if (bufferReady) {
    didSomething = true;
    execute();
  }

  return didSomething;
}

//////////////////////////////////////////////////////////////////////////////
//! @brief connect shell to a Stream (e.g. Serial, Serial2, ...)
//! @param requester the serial stream receiving commands
void SimpleSerialShell::attach(Stream & requester)
{
  shellConnection = &requester;
}

//////////////////////////////////////////////////////////////////////////////
/*! @brief pre-process backspace etc. on input line.
 * 
 * Arduino serial monitor appears to 'cook' lines before sending them
 * to output, so some of this is overkill.
 * 
 * But for serial terminals, backspace would be useful.
 *
 * @return true when buffer is ready to parse
 */
bool SimpleSerialShell::prepInput(void)
{
  bool bufferReady = false; // assume not ready

  int c = read();
  switch (c)
  {
    case -1: // No character present; don't do anything.
    case  0: // throw away NUL characters
      break;

    // Line editing characters
    case 127: // DEL delete key
    case '\b':  // CTRL(H) backspace
      // Destructive backspace: remove last character
      if (inptr > 0) {
        print("\010 \010");  // "\b \b" -- remove char in raw UI
        linebuffer[--inptr] = 0;
      }
      break;

    case 0x12: //CTRL('R')
      //Ctrl-R retypes the line
      print("\r\n");
      print(linebuffer);
      break;

    case 0x15: //CTRL('U')
      //Ctrl-U deletes the entire line and starts over.
      println("XXX");
      resetBuffer();
      break;

    case ';':   // BLE monitor apps don't let you add '\r' to a string, so ';' ends a command
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
      break;
  }

  return bufferReady;
}

//////////////////////////////////////////////////////////////////////////////
/*!
 * @brief process a string as if it were typed from console
 *  @param commandString a command with arguments; e.g. "echo hello"
 *  @return command exit code
 */
int SimpleSerialShell::execute(const char commandString[])
{
  // overwrites anything in linebuffer; hope you don't need it!
  strncpy(linebuffer, commandString, BUFSIZE);
  return execute();
}

//////////////////////////////////////////////////////////////////////////////
/*!
 * @brief process the buffer, find and execute command
 * @return command exit code
 */
int SimpleSerialShell::execute(void)
{
  char * argv[MAXARGS] = {0};
  linebuffer[BUFSIZE - 1] = '\0'; // play it safe
  int argc = 0;

  char * rest = NULL;
  const char * whitespace = " \t\r\n";
  char * commandName = strtok_r(linebuffer, whitespace, &rest);

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
    char * anArg = strtok_r(0, whitespace, &rest);
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
/*!
 * @brief execute command with arguments
 * @param argc number of arguments in araray (including command name)
 * @param argv array of argument strings for command to process
 * @return command exit code
 */
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
/*!
 * @brief report an error
 * @param constMsg error message
 * @param error code
 * @return error code
 */
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
/*!
 * @brief clear/reset the input line
 */
void SimpleSerialShell::resetBuffer(void)
{
  memset(linebuffer, 0, sizeof(linebuffer));
  inptr = 0;
}

//////////////////////////////////////////////////////////////////////////////
/*!
 *  @brief helper to print the list of commands
 * 
 * SimpleSerialShell::printHelp() is a static method.
 * It can access the shell's linked list of commands.
 *
 * printHelp() ignores its input arguments.
 * 
 * @param argc number of argument strings
 * @param argv array of argument strings
 * @return 0 (OK/no errors)
 */
int SimpleSerialShell::printHelp(int argc, char **argv)
{
  shell.println(F("Commands available are:"));
  auto aCmd = firstCommand;  // first in list of commands.
  while (aCmd)
  {
    shell.print(F("  "));
    shell.println(aCmd->name);
    aCmd = aCmd->next;
  }
  return 0;	// OK or "no errors"
}

///////////////////////////////////////////////////////////////
/*! @brief i/o stream support
 *  @param aByte a byte to write
 *  @return number of bytes written successfully (0 or 1)
 */
size_t SimpleSerialShell::write(uint8_t aByte)
{
  return shellConnection ?
         shellConnection->write(aByte)
         : 0;
}

//! @brief i/o stream support
//! @return 1 if stream has data available
int SimpleSerialShell::available()
{
  return shellConnection ? shellConnection->available() : 0;
}

//! @brief i/o stream support
//! @return a byte from stream
int SimpleSerialShell::read()
{
  return shellConnection ? shellConnection->read() : 0;
}

//! @brief i/o stream support
//! @return the next byte the stream will read
int SimpleSerialShell::peek()
{
  return shellConnection ? shellConnection->peek() : 0;
}

//! @brief i/o stream support
void SimpleSerialShell::flush()
{
  if(shellConnection)
    shellConnection->flush();
}
