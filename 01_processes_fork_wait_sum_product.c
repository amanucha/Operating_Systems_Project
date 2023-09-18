// Bibliography
// 1) W. Richard Stevens and Stephan A. Rago, "Advanced Programming
// in the Unix Environment", Addison Wesley, 2014, 3rd edition 
// 2) http://www.yolinux.com/ 

// Create 2 processes 
// a) parent process computes sum of 6 numbers
// b) child process computes product of 6 numbers
// Read the API for fork/wait from Chapter 9 of the book,
// mainly pages 228-234 and 238-239. Also see
// http://www.yolinux.com/TUTORIALS/ForkExecProcesses.htm
//

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void) {

int A[]={1,2,3,4,5,6}, i, sum=0, product=1, n;

        n=fork();
        if (n == -1) {
                  fprintf(stderr,"problem creating a process \n");
        }
        else if (n==0){ // child process
                for (i=0;i<6;i++) sum += A[i];
                printf("child process:%d computed the sum:%d \n", getpid(), sum);
                sleep(10);
                exit(0);
        }
        else if (n > 0) { // parent process
                for (i=0;i<6;i++)
                        product *= A[i];
                printf("parent process:%d completed the product: %d \
                         (child %d computes the sum)  \n", getpid(), product, n);
                sleep(10);
                wait(0);
        }
 
        return 0;
}
