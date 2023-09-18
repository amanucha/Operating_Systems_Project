// Sum of array elements with process and locks
// gcc sum_test.c to compile and ./a.out to run
// For locks see Section 12.4.1 (sync attributes).
//
// Read memory mapped I/O (mmap, munmap) in Section 14.8
// from Stevens and Rago, pages 430-433 and other pages
// for shm_open, ftruncate etc...

// For this part (posix shared memory), it's easier to consult
// Chapter 13, especially Sections 13.1-3, pages 337-341 from
// Stevens book
// For System V shared memory, you can also consult chapter 14,
// pages 352-360.)
// Access to Stevens book: 
// "https://raw.githubusercontent.com/AugustTan/documents/master/ebook - W. Richard Stevens - Unix Network Programming Vol2.pdf"
//
// To compile do: gcc 03_sum_test_process_lock_posix_shm_with_shm_unlink.c -lpthread -lrt


#define _GNU_SOURCE     // required by pthread_yield (do: man pthread_yield)

#define MUTEX "/lock"
#define SHMEM "/shmem"

#include <sys/ipc.h>    // interprocess communication header
#include <unistd.h>     // fork()
#include <sys/wait.h>   // wait(0)
#include <sys/shm.h>    // shared memory

#include <pthread.h>    // posix threads gia locks

#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>           /* For O_* constants */

#include <errno.h>

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NUMBER 10	// random numbers in [0 .. MAX_NUMBER-1]
#define N 10000		// array size
#define NPROC 100	// number of processes (including parent)

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
	int myid, segment_shmem;
	int *ptr;
	int count = 0;
	int temp;

	int segment_mutex;
	pthread_mutex_t *B0;

	int pfork(int);
	void pjoin(int ,int);

     	if(readmat(a, N) < 0) {
		printf("Array size is negative \n");
		exit(1);
	}

	//writemat(a, N);

	int mode = S_IRWXU | S_IRWXG;
	segment_mutex = shm_open(MUTEX, O_CREAT | O_RDWR | O_TRUNC, mode);
	segment_shmem = shm_open(SHMEM, O_CREAT | O_RDWR | O_TRUNC, mode);

	if ((segment_mutex < 0) || (segment_shmem < 0 ))
	{
		perror("failure on shm_open on mutex and/or shared memory segment");
		exit(1);
	}

	if (ftruncate(segment_mutex, sizeof(pthread_mutex_t)) == -1)
	{
		perror("Error on ftruncate to sizeof pthread_mutex_t\n");
		exit(-1);
	}

	if (ftruncate(segment_shmem, sizeof(int)) == -1)
	{
		perror("Error on ftruncate to sizeof shmem \n");
		exit(-1);
	}

	B0 = (pthread_mutex_t*) mmap(NULL, sizeof(pthread_mutex_t),
		PROT_READ | PROT_WRITE, MAP_SHARED, segment_mutex, 0);

	ptr = (int *) mmap(NULL, sizeof(int), 
		PROT_READ | PROT_WRITE, MAP_SHARED, segment_shmem, 0);

	if ((B0 == MAP_FAILED ) || (ptr == MAP_FAILED))
	{
		perror("Error on mmap on mutex and/or shared memory \n");
		exit(1);
	}

	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED); // use with processes
	//pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_PRIVATE); // use with pthreads (later)
	pthread_mutex_init(B0, &attr);

	srand(1L);

	// initialize value of virtual address pointer

	// create a child process
	myid=pfork(NPROC-1);

	// sum every NPROC terms in starting from myid element
        for (i=myid;i<N;i+=NPROC) {
        	pthread_mutex_lock(B0); // lock
			temp=*ptr;
			printf("%d_(ptr:%d, a[%d]:%d) ", getpid(), temp, i, a[i]);
			fflush(stdout);
			temp+=a[i];
			pthread_yield();
			*ptr=temp;
			//*ptr=*ptr+a[i];
			count ++;
			printf("new_ptr:%d (round:%d times) \n", *ptr, count);
			fflush(stdout);
        	pthread_mutex_unlock(B0); // unlock
	}
	//printf("%d_process_tot_sum: %d (in shared memory) \n", getpid(), *ptr);

	// wait for all children to terminate before accessing *ptr
	pjoin(NPROC-1,myid);

	printf("%d_parent_final_sum: %d (in shared memory) \n", getpid(), *ptr);
	fflush(stdout);

        pthread_mutex_destroy(B0); // destroy dynamically allocated mutex

	if (munmap(B0, sizeof(pthread_mutex_t)) == -1) {
		printf("Unmap of mutex failed: %s\n", strerror(errno));
		exit(1);
	}

	if (munmap(ptr, sizeof(pthread_mutex_t)) == -1) {
		printf("Unmap of shared memory failed: %s\n", strerror(errno));
		exit(1);
	}

  	/* close the shared memory mutex as if it was a file */
  	if (close(segment_mutex) == -1) {
    		printf("segment of mutex: close failed: %s\n", strerror(errno));
    		exit(1);
  	}

  	/* close the shared memory segment as if it was a file */
  	if (close(segment_shmem) == -1) {
    		printf("segment of shared memory: close failed: %s\n", strerror(errno));
    		exit(1);
  	}
  	if (shm_unlink(MUTEX) == -1) {
    		printf("segment of shared memory: close failed: %s\n", strerror(errno));
    		exit(1);
  	}
  	if (shm_unlink(SHMEM) == -1) {
    		printf("segment of shared memory: close failed: %s\n", strerror(errno));
    		exit(1);
  	}


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
