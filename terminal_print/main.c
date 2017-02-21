#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char **argv)
{
  int i = 0;

  fprintf(stdout, "(%.3d)", i);
  fflush(stdout);
  while (i < 200)
  {
    fprintf(stdout, "\b\b\b\b%.3d)", i);
    fflush(stdout);
    sleep(1);
    i++;
  }
  fprintf(stdout, "\n");

  return EXIT_SUCCESS;
}
