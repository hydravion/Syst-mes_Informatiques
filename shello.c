#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <linux/limits.h>

#define USER_INPUT_MAX_SIZE 1000
#define CWD_BUF_MAX_SIZE 1000
#define ARGV_SIZE 30



int printCWD() {
  /* prints the current working directory before prompting the user for input*/

  static char cwd_buf[CWD_BUF_MAX_SIZE];

  getcwd(cwd_buf, CWD_BUF_MAX_SIZE);

  if (errno == ERANGE) {
    printf("Error: printCWD() - Current working directory is too large\n");
  }
  else {
    printf("%s$ ", cwd_buf);
  }
  return 0;
}

char *readUserInput() {
  /* reads the user input from stdin */

  static char user_input[USER_INPUT_MAX_SIZE];
  memset(user_input, 0, sizeof(char) * USER_INPUT_MAX_SIZE);  // (re)initialize the
                                                              // user_input array

  printCWD();
  scanf("%s", user_input);  // BUG: try writing "salut ca va"
                            // and it will printCWD() 3 times.. why ?

  return user_input;
}

char **parseInput(char *user_input) {
  /* parses the user input to produce an argv array containing the different
    strings from the input */

  static char *argv[ARGV_SIZE]; // the argv array
  memset(argv, 0, sizeof(char*) * ARGV_SIZE); // (re)initialize the
                                              // user_input array

  char *str, *token;
  int j;

  // TODO: deal with background jobs ("&" character)

  // inspired by man strtok
  for (j = 0, str = user_input ; ; j++, str = NULL) {
    token = strtok(str, " \t"); // split the input string (delim bytes are whitespace and tab)
    if (token == NULL)
      break;

    argv[j] = token; // each argv element contains a string
  }

  return argv;
}


int isCmdABuiltin(char **argv) {
  /* determines whether the user's command is a builtin */

  static char *builtins[] = {
    "cd",
    "exit"
  };

  int i;
  int array_size = sizeof(builtins) / sizeof(char*);  // TODO? Change this if
                                                      // array is dynamically
                                                      // allocated later on

  for (i = 0 ; i < array_size ; i++) {
    if (strcmp(argv[0], builtins[i]) == 0) // if the command is a builtin
      return 1;
  }
  return 0;
}
/*
int evalBuiltin() {}
int evalJob() {}
*/

int evalInput(char *user_input) {
  /*  evaluates the user input. It checks for a builtin and evaluates it accordingly
      if one is found. Otherwise it evaluates the command as a job. */

  char **argv;

  argv = parseInput(user_input);

  if (isCmdABuiltin(argv)) {
    // evalBuiltin(argv);
  }
  else {
    // evalJob(argv);
  }

  return 0;
}

int printResult() {
  /* ?? prints Foreground job exited ?.. */

  return 0;
}

int readEvalPrintLoop() {
  /* reads the user input, evaluates it and prints the result of it. */

  char *user_input;

  do {
    user_input = readUserInput();
    evalInput(user_input);
    printResult();
  } while(1);

  return 0;
}

int main(int argc, char *argv[]) {

  readEvalPrintLoop(); // Starts the REPL

  exit(EXIT_SUCCESS);
}
