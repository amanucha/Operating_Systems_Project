// Sum of an array using processes (fork/wait)
// Do "gcc sum_test.c" to compile,
// and then "./a.out" to run
// Read more from
// http://www.yolinux.com/TUTORIALS/ForkExecProcesses.html
//
// Also for shared memory, see Section 15.9 (and 15.6.1 for the ftok function)
// from W. Richard Stevens and Stephan A. Rago, "Advanced Programming in the
// Unix Environment", Addison Wesley, 2014, 3rd edition 

// 1): Can you etend the code to more threads?
// 2): Can you extract stats from 3rd argument of shmctl (struct shmid-ds)

#include <sys/ipc.h>    // interprocess communication header
#include <unistd.h>     // fork()
#include <sys/wait.h>   // wait(0) 
#include <sys/shm.h>    // shared memory
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_NUMBER 10	// random numbers in [0 .. MAX_NUMBER-1]
#define N 10		// array size

/* Function that fills array a[] with integer random numbers
   */
int readmat(int a[],int n)
{
	long i;

	if (n > 0) {
		for (i=0;i<n;i++)
			a[i]= rand() % MAX_NUMBER;
		return(0);
	}
	return(-1);
}

/* Function that prints array a[]
   */
int writemat(int a[],int n)
{
	long i;

	if (n > 0) {
		for (i=0;i<n;i++)
			printf(" a[%d] = %d \n", i, a[i]);
		return(0);
	}
	return(-1);
}

int main()
{
	// array with elements
	int a[N];
	int sum=0, i;
	int *ptr;
	int myid, shmid;

	int pfork(int);
	void pjoin(int ,int);


	srand(1L);
     	if(readmat(a, N) < 0) {
		printf("Array size is negative \n");
		exit(1);
	}

	writemat(a, N);

	// allocate a memory segment (pointer to physical memory
	// check with ipcs command
	shmid=shmget(IPC_PRIVATE,4,IPC_CREAT|0666);
	if(shmid == -1) {
		printf("ERROR\n");
		exit(1);
	}
	else
		printf("Shmid Is %d\n",shmid);
    
	// attach memory segment to address space to get virtual address pointer
	ptr=shmat(shmid,0,0);

	// initialize value of virtual address pointer
	*ptr=0;    

	// pfork(N) creates N children with myid: 1 to N, parent has myid=0
	myid=pfork(1);

	if(myid==0) { // parent
		// sum every two terms starting from zero array element
		for(i=0;i<N;i+=2)
		{
			sum=sum+a[i];
		}
		printf("Sum in parent is %d\n",sum);
	}
	else if(myid==1) { // child with id1 (see pfork)
		// sum every two terms starting from first array element
		for(i=1;i<N;i+=2)
		{
				*ptr=*ptr+a[i];
		}
		printf("Sum in child is %d\n",*ptr);
	}

	// parent waits for all children (who perform exit), before accessing *ptr (and sum)
	pjoin(1,myid);

	sum=sum+*ptr;

	system("ipcs");

        shmdt(ptr);

        shmctl(shmid, IPC_RMID, NULL);

        sleep(20); // time so that we can view it has been erased

	system("ipcs");

	printf("Total sum is %d\n",sum);
	return 0;
}

int pfork(int x)
{
	int j;

	for (j=1;j<=x;j++) {
		if(fork()==0)
			return j;
	}
	return 0;
}

void pjoin(int x,int id)
{
int j;

	if (id==0) {
		for(j=1;j<=x;j++) {
			wait(0);
		}    
	}
	else {
		exit(0);
	}
}
