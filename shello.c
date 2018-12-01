#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <linux/limits.h>


#define USER_INPUT_MAX_SIZE 1000
#define CWD_BUF_MAX_SIZE 1000
#define ARGV_SIZE 30
#define HOSTNAME_MAX_SIZE 50

#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */
#define BOLDBLACK   "\033[1m\033[30m"      /* Bold Black */
#define BOLDRED     "\033[1m\033[31m"      /* Bold Red */
#define BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
#define BOLDYELLOW  "\033[1m\033[33m"      /* Bold Yellow */
#define BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m"      /* Bold Magenta */
#define BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */
#define BOLDWHITE   "\033[1m\033[37m"      /* Bold White */

int printCWD() {
  /* prints the current working directory as part of the user prompt*/

  static char cwd_buf[CWD_BUF_MAX_SIZE];

  getcwd(cwd_buf, CWD_BUF_MAX_SIZE);

  if (errno == ERANGE) {
    fprintf(stderr, "Error: printCWD() - Current working directory is too large\n");
  }
  else {
    printf(BOLDBLUE "%s$ " RESET, cwd_buf);
  }
  return 0;
}

int printUserAndHostName() {
  static char *username = NULL;
  static char hostname[HOSTNAME_MAX_SIZE];

  // get the username if not already defined
  #ifndef IFNDEF_USERNAME
    username = getlogin();

    if (username == NULL) {
      fprintf(stderr, "printUserAndHostName(): couldn't get username.\n");
    }
    else {
      #define IFNDEF_USERNAME
    }
  #endif

  // get the hostname if not already defined
  #ifndef IFNDEF_HOSTNAME
    if (gethostname(hostname, HOSTNAME_MAX_SIZE) < 0) {
      fprintf(stderr, "printUserAndHostName(): couldn't get hostname.\n%s\n", strerror(errno));
    }
    else {
      #define IFNDEF_HOSTNAME
    }
  #endif

  // if both are defined print them as part of the prompt
  #if defined(IFNDEF_HOSTNAME) && defined(IFNDEF_USERNAME)
    printf(BOLDGREEN "%s@%s" RESET ":", username, hostname);
  #endif

  return 0;
}

int printPrompt() {
  printUserAndHostName();
  printCWD();

  return 0;
}

char *readUserInput() {
  /* reads the user input from stdin */

  static char user_input[USER_INPUT_MAX_SIZE];
  memset(user_input, 0, sizeof(char) * USER_INPUT_MAX_SIZE);  // (re)initialize the
                                                              // user_input array

  printPrompt();

  if (fgets(user_input, USER_INPUT_MAX_SIZE, stdin) == NULL) {
    fprintf(stderr, "readUserInput(): an error occured while reading user input.\n");
  }
  user_input[strcspn(user_input, "\n")] = 0;  // snippet found on stackoverflow,
                                              // removes the trailing newline character
                                              // so that strcmp works correctly later on.

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

int evalBuiltin(char **argv) {
  /* evaluates builtins */

  if (strcmp(argv[0], "cd") == 0) {
      chdir(argv[1]);
    }
  else if (strcmp(argv[0], "exit") == 0) {
    exit(EXIT_SUCCESS); // TODO: deal with background jobs and signals
  }
  else {
    fprintf(stderr, "evalBuiltin(): builtin not implemented.\n");
  }

  return -1;
}


int evalJob(char **argv) {
  /*  */

  // found on/inspired by the internet, man waitpid
  pid_t pid, w;

  pid = fork();

  if (pid == -1) {
    fprintf(stderr, "evalJob(): failed to fork().\n");
  }
  else if (pid == 0) { // executed by the child
    execvpe(argv[0], argv, NULL);
  }
  else {
    int wstatus;
    w = waitpid(pid, &wstatus, 0);
    if (WIFEXITED(wstatus)) {
      return w;
    }
    else {
      fprintf(stderr, "evalJob(): waitpid(): child didn't terminate normally.\n");
    }
  }

  return -1;
}


int evalInput(char *user_input) {
  /*  evaluates the user input. It checks for a builtin and evaluates it accordingly
      if one is found. Otherwise it evaluates the command as a job. */

  char **argv;

  argv = parseInput(user_input);

  if (isCmdABuiltin(argv)) {
    evalBuiltin(argv);
  }
  else {
    return evalJob(argv);
  }

  return -1;
}

int printResult(pid_t pid_child) {
  /* ?? prints Foreground job exited ?.. */
  if (pid_child < 0) {
    return 0;
  }
  printf("Foreground job exited with code %d.\n", pid_child);

  return 0;
}
void printWelcomeMessage() {
  printf(BOLDYELLOW "Welcome to the...\n\n");
  printf(BOLDMAGENTA
  "      ___           ___           ___           ___       ___       ___     \n"
  "     /\\  \\         /\\__\\         /\\  \\         /\\__\\     /\\__\\     /\\  \\    \n"
  "    /::\\  \\       /:/  /        /::\\  \\       /:/  /    /:/  /    /::\\  \\   \n"
  "   /:/\\ \\  \\     /:/__/        /:/\\:\\  \\     /:/  /    /:/  /    /:/\\:\\  \\  \n"
  "  _\\:\\~\\ \\  \\   /::\\  \\ ___   /::\\~\\:\\  \\   /:/  /    /:/  /    /:/  \\:\\  \\ \n"
  " /\\ \\:\\ \\ \\__\\ /:/\\:\\  /\\__\\ /:/\\:\\ \\:\\__\\ /:/__/    /:/__/    /:/__/ \\:\\__\\\n"
  " \\:\\ \\:\\ \\/__/ \\/__\\:\\/:/  / \\:\\~\\:\\ \\/__/ \\:\\  \\    \\:\\  \\    \\:\\  \\ /:/  /\n"
  "  \\:\\ \\:\\__\\        \\::/  /   \\:\\ \\:\\__\\    \\:\\  \\    \\:\\  \\    \\:\\  /:/  / \n"
  "   \\:\\/:/  /        /:/  /     \\:\\ \\/__/     \\:\\  \\    \\:\\  \\    \\:\\/:/  /  \n"
  "    \\::/  /        /:/  /       \\:\\__\\        \\:\\__\\    \\:\\__\\    \\::/  /   \n"
  "     \\/__/         \\/__/         \\/__/         \\/__/     \\/__/     \\/__/    \n"
  RESET);
  printf(BOLDCYAN "\n\nshell.\nEnjoy your stay! :)\n" RESET);
}

int runReadEvalPrintLoop() {
  /* reads the user input, evaluates it and prints the result of it. */

  char *user_input;
  pid_t pid_child;

  printWelcomeMessage();

  do {
    user_input = readUserInput();
    pid_child = evalInput(user_input);
    printResult(pid_child);
  } while(1);

  return 0;
}

int main(int argc, char *argv[]) {

  runReadEvalPrintLoop(); // Starts the REPL

  exit(EXIT_SUCCESS);
}
