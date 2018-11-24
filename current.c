#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <linux/limits.h>

int main(int argc, char **argv){
  char test[] = "tester Steve";
  char test2[] = "Moi aussi et j'ai m3m3 2 chiffres et des fkdjs inconnus s";
  char *keep;
  const char *delim = " \t";
  int count = 0;
  char testCopy[40];
  strcpy(testCopy, test);
  char pathName[PATH_MAX];



  /*
  printf("%ld\n", sizeof(&arg));
  printf("%ld\n", sizeof(arg));
  printf("%ld\n", sizeof(*arg));
  printf("%ld\n", sizeof(**arg));
  printf("%ld\n", sizeof(*(test + 5)));
  printf("%ld\n", sizeof(*test2));
  */


  keep = strtok(testCopy, delim);
  while (keep != NULL){
    keep = strtok(NULL, delim);
    count++;
  }

  char **arg = malloc( sizeof(char*) * count);


  count = 0;
  keep = strtok(test, delim);
  while (keep != NULL){
    arg[count] = keep;
    keep = strtok(NULL, delim);
    count++;
  }


  if (strcmp(arg[0], "exit") == 0){
    exit(EXIT_SUCCESS);
  }

  else if(strcmp(arg[0], "cd") == 0){
    if (count <= 1){
      perror("PATH needed");
    }
    chdir(arg[1]);
    getcwd(pathName, PATH_MAX);
    printf("%s\n", pathName);
  }

  else{
    execve(arg[0], arg, NULL);
  }

    printf("yo\n");
    exit(EXIT_SUCCESS);
}
