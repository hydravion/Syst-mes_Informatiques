#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

int main(int argc, char **argv){
  if (argc != 2){
    printf("USAGE : %s <string>\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  printf("Hello %s\n", argv[1]);


  exit(EXIT_SUCCESS);
}
