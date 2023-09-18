// Bibliography
// 1) W. Richard Stevens and Stephan A. Rago, "Advanced Programming
// in the Unix Environment", Addison Wesley, 2014, 3rd edition 
// 2) // http://www.yolinux.com/ 

// Create 2 processes 
// a) parent process computes sum of 6 numbers
// b) child process computes product of 6 numbers
// Read the API for fork/wait from Chapter 9 of the book,
// mainly pages 228-234 and 238-239. Also see
// http://www.yolinux.com/TUTORIALS/ForkExecProcesses.htm

// This code tangles the following question:
// What will happen if the parent dies before the child terminates?
// See the code (2nd child kills the parent process)
//
// You can also experiment with kill -9 signal externally. Same results!

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void) {

int A[]={1,2,3,4,5,6}, i, sum=0, product=1, m, n;
char buf[41];

	m = fork();
	if (m == -1) {
		  fprintf(stderr,"problem creating a process \n");
	}
	else if (m==0){ // 1st child process computes sum
		printf("1st child process:%d (of parent:%d) -> computing sum, starting value:%d \n", getpid(), getppid(), sum);
		for (i=0;i<6;i++) sum += A[i];
		sleep(20);
		printf("1st child prcocess:%d (of parent:%d) -> computed sum:%d \n", getpid(), getppid(), sum);
		exit(0);
	}
	else if (m > 0) { // parent process
		n = fork();
		if (n == -1) {
			  fprintf(stderr,"problem creating a process \n");
		}
		else if (n==0){ // 2nd child computes product, kills 1st child, and monitors parent process
			printf("2nd child process:%d (of parent:%d) -> computing product, starting value:%d \n", getpid(), getppid(), product);
			for (i=0;i<6;i++) product *= A[i];

			// viewing process status
               		sprintf(buf, "ps -adefljc | head -1");
			system(buf);
               		sprintf(buf, "ps -adefljc | grep a.out | grep -v grep");
			for (i=0; i< 5; i++) {
				system(buf);
				system("/usr/bin/echo --"); 
				sleep (1);
			}
			// sending kill to parent
               		sprintf(buf, "kill -9 %ld", getppid());
			system(buf);

			// viewing process status
               		sprintf(buf, "echo \n");
			system(buf);
               		sprintf(buf, "ps -adefljc | head -1");
			system(buf);
               		sprintf(buf, "ps -adefljc | grep a.out | grep -v grep");
			for (i=0; i< 5; i++) {
				system(buf);
				sleep (1);
			}
			sleep(10);

			printf("2nd child process:%d (of parent:%d) -> computed product:%d \n", getpid(), getppid(), product);
			exit(0);
		}
		else if (n > 0) { // parent process

			printf("parent:%d waits for its children (1st child:%d computes sum, 2nd child:%d computes product) \n", getpid(), m, n);
			printf("parent:%d will be killed, in this case by its 2nd child:%d \n", getpid(), n);
			wait(0);
			wait(0);
		}
	}
 
	return 0;
}
