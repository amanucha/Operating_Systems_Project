// second pipe example: fork/read/write within same process
// see Sections 15.1-3 in W. Richard Stevens and Stephan A. Rago,
// Advanced Programming in the Unix Environment, Addison Wesley, 
// 2014, 3rd edition.

// This is a simple pipe example: read/write within same process
//          	(not commonly used this way)

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define MSGSIZE  8 // up to 8 characters

char *msg1 = "hello #1";
char *msg2 = "hello #2";
char *msg3 = "hello #3";

int main()
{

char inbuf[MSGSIZE]; // define input buffer
int p[2]; // standard pipe definition -> file descriptor p[0] for reading and p[1] for writing
int j;

   /* open pipe with the p[2] descriptor */
   if(pipe(p) == -1)
   {    perror("pipe call error");
        exit(1);
   }
  
   // now pipe is created, so write the pipe p[1] descriptor
   write(p[1], msg1, MSGSIZE);
   write(p[1], msg2, MSGSIZE);
   write(p[1], msg3, MSGSIZE);

   // now read the pipe p[0] descriptor
   for(j=0; j<3; j++)
   {
      read(p[0], inbuf, MSGSIZE);
      printf("p_0:%s\n", inbuf);
   }

   exit(0);
}
