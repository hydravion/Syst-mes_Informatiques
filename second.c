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

  if(arg[0][0] != '\0'){
    pid_t pid = fork();
    if (pid == 0){
      printf("Stdin is descriptor %d\n", fileno(stdin));
      freopen("/dev/null", "r", stdin);
      printf("Stdin is now /dev/null and hopefully still fd %d\n",
         fileno(stdin));
         execve(arg[0], arg, NULL);
      freopen("/dev/stdin", "w", stdin);
      printf("Now we put it back, hopefully its still fd %d\n",
         fileno(stdin));
    }
    else {


        }

        // aller dans le dossier /bin (en testant les différentes concaténations)

    }
    memset(test, 0, sizeof test);
    memset(testCopy, 0, sizeof testCopy);
    free(arg);
  }
}
