#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int main () {
  int pid;
  printf ("original process having: PID %d and PPID %d.\n", getpid (), getppid ());
  pid = fork (); /* FORK */
  if (pid != 0) { /* pid is non-zero, parent */
    printf ("parent process having: PID %d and PPID %d.\n", getpid (), getppid ());
    printf ("My child’s PID is %d.\n", pid);
    sleep(5); //verificare senza sleep: padre potrebbe terminare prima di figlio
  }
  else { /* pid is zero, so I must be the child */
    printf ("child process having: PID %d and PPID %d.\n", getpid (), getppid ());
  }
  printf ("PID %d terminates.\n", getpid () ); /* both print this */
  return 0;
}
