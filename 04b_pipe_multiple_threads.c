// second pipe example: fork/read/write within same process
// see Sections 15.1-3 in W. Richard Stevens and Stephan A. Rago,
// Advanced Programming in the Unix Environment, Addison Wesley, 
// 2014, 3rd edition.

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

/* Read characters from the pipe and echo them to stdout. */
void read_from_pipe (int file)
{
  FILE *stream;
  int c;
  stream = fdopen (file, "r");
  while ((c = fgetc (stream)) != EOF) {
    printf(" \n ");
    putchar (c);
  }
  fclose (stream);
}

/* Write some random text to the pipe. */
void write_to_pipe (int file)
{
  // use strem instead of char inbuf[MSGSIZE]
  FILE *stream;
  stream = fdopen (file, "w");
  fprintf (stream, "hello, world!\n");
  fprintf (stream, "goodbye, world!\n");
  fclose (stream);
}

int main (void)
{

pid_t pid; // pid used to get fork() return value
int mypipe[2]; // standard pipe definition -> file descriptor p[0] for reading and p[1] for writing


  /* Create the pipe using pipe */
  if (pipe (mypipe)) { /* open pipe with the p[2] descriptor */
      fprintf (stderr, "Pipe failed.\n");
      return EXIT_FAILURE;
  }

  /* now pipe is created, so fork to create double linked pipe */
  /* Create the child process. */
  pid = fork ();
  if (pid == (pid_t) 0) { // child process

      /* close child write to pipe - so only parent can write to pipe (child can read from it) */
      close (mypipe[1]);

      read_from_pipe (mypipe[0]);
      close (mypipe[0]);
      return EXIT_SUCCESS;
  }
  else if (pid < (pid_t) 0) {

      /* The fork failed */
      fprintf (stderr, "Fork failed.\n");
      return EXIT_FAILURE;
  }
  else { // parent  process

      /* close parent read from pipe - so only child can read from pipe (parent can write to it) */
      close (mypipe[0]);

      write_to_pipe (mypipe[1]);
      close (mypipe[1]);
      return EXIT_SUCCESS;
  }
}
