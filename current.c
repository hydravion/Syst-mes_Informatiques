#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <linux/limits.h>
#include <sys/types.h>
#include <sys/wait.h>

// FONCTIONS

int compter(char *testCopy, const char *delim){
  char *keep;
  int count = 0;
  keep = strtok(testCopy, delim);
  while (keep != NULL){
    keep = strtok(NULL, delim);
    count++;
  }
  return count;
}

void parseAndStock(char **arg, char* test, const char *delim){
  char *keep;
  int count = 0;
  keep = strtok(test, delim);
  while (keep != NULL){
    arg[count] = keep;
    keep = strtok(NULL, delim);
    count++;
  }

}

int main(int argc, char **argv){
  const char *delim = " \t\n";
  char pathName[PATH_MAX];
  char test[60];
  char testCopy[60];
  int last;

  while(1){

    printf("Shell$: ");
    fgets(test, 60, stdin);
    last = strlen(test);

    // ici réussir à récupérer de la ligne de commande
    // à faire plus tard : détecter les & et gérer ça .

    // comment récupérer les arguments de ligne de commande ?
    strcpy(testCopy, test);
    int count = 0;
    count = compter(testCopy, delim);
    char **arg = malloc( sizeof(char*) * count);
    parseAndStock(arg, test, delim);
    count = 0;


    // built-in

  if (strcmp(arg[0], "exit") == 0){
    printf("Adieu monde cruel\n");
    exit(EXIT_SUCCESS);
  }

  else if(strcmp(arg[0], "cd") == 0){
    if (arg[1] == NULL){
      perror("PATH needed");
    }
    printf("%s\n", arg[1]);
    chdir(arg[1]);
    getcwd(pathName, PATH_MAX);
    printf("%s\n", pathName);
  }

  else if(arg[0][0] != '\0'){
    pid_t pid = fork();
    if (pid > 0){
      int wstatus;
      wait(&wstatus);
      int ans = WIFEXITED(wstatus);
      printf("Foreground job exited with code %d\n", wstatus);
    }
    if (pid == 0){
      execve(arg[0], arg, NULL);
        }

        // aller dans le dossier /bin (en testant les différentes concaténations)

    }
    memset(test, 0, sizeof test);
    memset(testCopy, 0, sizeof testCopy);
    free(arg);
  }
}
