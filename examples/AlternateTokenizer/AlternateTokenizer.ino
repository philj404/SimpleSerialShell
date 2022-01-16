/**
 * @file AlternateTokenizer.ino
 * @author brucemack
 * @version 0.1
 * @date 2021-12-29
 * 
 * @copyright Copyright (c) 2021
 * 
 * A demonstration of the SimpleSerialShell using an alternate tokenizer that 
 * supports the use of quoted strings.
 * 
 * Depends on https://github.com/philj404/SimpleSerialShell.
 * 
 * Here is an example run:
 * 
 * echo test
 * Argument 1: [test]
 * echo test test2
 * Argument 1: [test]
 * Argument 2: [test2]
 * echo test "this is a long test"
 * Argument 1: [test]
 * Argument 2: [this is a long test]
 */
#include <SimpleSerialShell.h>

/**
 * @brief Checks to see if a character is in a string.
 *
 * @param needle The character being searched for
 * @param haystack null-delimited string
 * @return true If found.
 */
static bool isIn(char needle, const char* haystack) {
  const char* ptr = haystack;
  while (*ptr != 0) {
    if (needle == *ptr) {
      return true;
    }
    ptr++;
  }
  return false;
}

/**
 * @brief The tokenizer function that conforms to the SimpleSerialShell signature
 * requirement.  Please see strtok_r(3) for an explanation of the expected semantics.
 * 
 * @param str 
 * @param delims 
 * @param saveptr 
 * @return char* 
 */
static char* tokenizer(char* str, const char* delims, char** saveptr) {

  // Figure out where to start scanning from
  char* ptr = 0;
  if (str == 0) {
    ptr = *saveptr;
  } else {
    ptr = str;
  }
  
  // Consume/ignore any leading delimiters
  while (*ptr != 0 && isIn(*ptr, delims)) {
    ptr++;
  }

  // At this point we either have a null or a non-delimiter
  // character to deal with. If there is nothing left in the 
  // string then return 0
  if (*ptr == 0) {
    return 0;
  }

  char* result;

  // Check to see if this is a quoted token 
  if (*ptr == '\"') {
    // Skip the opening quote
    ptr++;
    // Result is the first eligible character
    result = ptr;
    while (*ptr != 0) {
      if (*ptr == '\"') {
        // Turn the trailing delimiter into a null-termination
        *ptr = 0;
        // Skip forward for next start
        ptr++;
        break;
      } 
      ptr++;
    }
  } 
  else {
    // Result is the first eligible character
    result = ptr;
    while (*ptr != 0) {
      if (isIn(*ptr, delims)) {
        // Turn the trailing delimiter into a null-termination
        *ptr = 0;
        // Skip forward for next start
        ptr++;
        break;
      } 
      ptr++;
    }
  }

  // We will start on the next character when we return
  *saveptr = ptr;
  return result;
}

/**
 * @brief Installed into the shell as a command processor.  Prints out the 
 * arguments passed to the function.
 * 
 * @param argc 
 * @param argv 
 * @return int 
 */
int echo(int argc, char **argv) { 
    for (int i = 1; i < argc; i++) {
        shell.print("Argument ");
        shell.print(i);
        shell.print(": [");
        shell.print(argv[i]);
        shell.println("]");
    }
    return 0;
}

void setup() {
    Serial.begin(115200);
    shell.attach(Serial);
    shell.addCommand(F("echo [text tokens]"), echo);
    shell.setTokenizer(tokenizer);
}

void loop() {
    shell.executeIfInput();
}
