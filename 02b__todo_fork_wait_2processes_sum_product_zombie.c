// Bibliography
// 1) W. Richard Stevens and Stephan A. Rago, "Advanced Programming
// in the Unix Environment", Addison Wesley, 2014, 3rd edition 
// 2) http://www.yolinux.com/ 


// Create 2 processes 
// Parent computes sum, child computes product of 6 numbers
// What will happen if parent forgets to call wait(0)
//    (read about orphan and zombie processes)
//
// Read the API for fork/wait sunarthseis from Chapter 9 of the book,
// mainly pages 228-234 and 238-239. Also see
// http://www.yolinux.com/TUTORIALS/ForkExecProcesses.html

// Zombies stay for a long time?
// What will happen if you kill the parent process? Does it help?
//
// What will happen if you send a SIGCHLD signal to the parent process?
// Try sending: kill -s SIGCHLD ppid (to parent)

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void) {

int A[]={1,2,3,4,5,6}, i, sum=0, product=1, pid;
char buf[20];

	pid=fork();
	if (pid == -1) {
		  fprintf(stderr,"problem creating a process \n");
	}
	else if (pid==0){ // child process
		printf("This is child process %d (of parent %d) -> computng sum %d \n", getpid(), getppid(), sum);
		for (i=0;i<6;i++) sum += A[i];
		printf("This is child process %d (of parent %d) -> computed sum %d \n", getpid(), getppid(), sum);
		sleep(5);
		exit(0);
	}
	else if (pid > 0) { // parent process
		for (i=0;i<6;i++)
			product *= A[i];
		printf("The parent process %d completed the product %d (child %d computes the sum)  \n", getpid(), product, pid);
		for (i=0;i<100000;i++) {
			sprintf(buf, "ps %ld", (long int)pid);
			system(buf);
                        //sprintf(buf, "ps -adefljc | grep a.out | grep -v grep");
			//system("ps aux | grep a.out | grep -v grep");
          		//system("ps -Ao pid,tt,user,fname,tmout,f,wchan | grep a.out | grep -v grep");
			usleep(2000);
		}
	}
 
	return 0;
}
