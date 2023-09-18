#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <math.h>

#define BUFFER_SIZE 1024
#define MAX_VALUES 10

FILE *fp = NULL;
int terminate = 0;

void sigintHandler(int sig_num){
    terminate = 1;
    signal(SIGINT, sigintHandler);
    printf("\nTerminating telnet connection...\n");   
    if (fp != NULL) {
        pclose(fp);
    }
    exit(0);
}

int main(int argc, char **argv) {
    int i;
    printf("@focus off\n"); 		
    printf("@g0 on\n");                     
    printf("@default linewidth 3\n");
    printf("@default linestyle 1\n");
    printf("@default color 4\n");
    printf("@default pattern 1\n");
    printf("@default font 2\n");
    printf("@default char size 1.000000\n");
    printf("@default symbol size 0.23\n");
    printf("@background color 9\n");

    printf("@frame background color 5\n");
    printf("@frame background pattern 3\n");
    printf("@with g0\n");                   
    printf("@view 0.1, 0.1, 1.2, 0.9\n");   
    printf("@subtitle \" Power vs Time \"\n");     

    signal(SIGINT, sigintHandler); 

    fp = popen("telnet 192.168.4.1 23", "r");

    if (fp == NULL) {
        printf("Failed to open pipe.\n");
        return 1;
    }

    char buffer[BUFFER_SIZE];
    char* token;
    float values[MAX_VALUES];
    int k = 0; 
    int status = 0;
    int first_run = 1;
		    printf("@with g0\n");               
		    printf("@kill s0\n");
    while(!terminate) { 
        //discard the first two lines
	fgets(buffer, BUFFER_SIZE, fp);
	fgets(buffer, BUFFER_SIZE, fp);
	fgets(buffer, BUFFER_SIZE, fp);
	
        while (fgets(buffer, BUFFER_SIZE, fp) && !terminate) {
           //printf("inside OF WHILE:");
	   //printf("%s", buffer);
            if (strlen(buffer) > 0 && k < MAX_VALUES) {

		float value1;
		float value2;
		float value3;
		float value4;

		sscanf(buffer, "%f, %f, %f, %f", &value1, &value2, &value3, &value4);

		if(status){ //k==0
		    printf("@with g0\n");               
		    printf("@kill s0\n");
			sleep(5);
		    printf("%d %f\n", k-1, value3);
		    fflush(stdout);
		    status = 0;	
		}else{
		    printf("%d %f\n", k, value3);
		    fflush(stdout);		
		}
		if(k==9){
			printf("&\n");
			k = 0;
			status = 1;

			if (first_run) {
			    printf("@autoscale\n"); 
			    first_run = 0;         
		        }
		        printf("@redraw\n");
		        printf("@sleep 0.1\n");
		}   
            }
	    	k++;
	    

        }
    }
    pclose(fp);
    exit(0);
    return 0;
}
