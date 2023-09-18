#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <math.h>

#define BUFFER_SIZE 1024
#define MAX_VALUES 10
#define DATA_FILE_NAME "data.txt"
#define MAX_LINE_LENGTH 100

FILE *fp = NULL;
int terminate = 0;

void sigintHandler(int sig_num){
    terminate = 1;
    signal(SIGINT, sigintHandler);
    printf("\nTerminating telnet connection...\n");   
    //if (fp != NULL) {
        pclose(fp);
    //}
    system("pkill -9 telnet"); 
    exit(0);
}

int main(int argc, char **argv) {
    int i;





    printf("@focus off\n"); 		    /* turn of the focus markers (annoying) */
    printf("@g0 on\n");                     /* Activate graph 0 */

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

    printf("@with g0\n");                   /* reset the current graph to graph 0 */
    printf("@view 0.1, 0.1, 1.2, 0.9\n");   /* set the viewport for this graph */
    printf("@subtitle \" A vs B \"\n");     /* set the subtitle */



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
    int written = 0;
    int first_run = 1;
    int first_run2 = 1;
    while(!terminate) {
        fseek(fp, 0, SEEK_END);
	printf("@with g0\n");               /* reset the current graph to graph 0 */
	printf("@kill s0\n");               /* make sure s0 is available
                                             * (data will be read into this set)
                                             */
        while (fgets(buffer, BUFFER_SIZE, fp) && !terminate) {
	    //printf(buffer);
            if (strlen(buffer) > 0 && k < MAX_VALUES) {
		if(first_run2){
		    k = k-1;
	            first_run2 = 0;
		}
		char* first = strtok(buffer, ", ");
		char* second = first ? strtok(NULL, ", ") : NULL;
                token = second ? strtok(NULL, ", ") : NULL;
		if(token){
		   values[k++] = atof(token); 
		}else{
		    //printf("Could not read the Watt value from telnet");		
		}  
            } else {
                FILE *data_file = fopen(DATA_FILE_NAME, "w");

                if (data_file == NULL) {
                    printf("Failed to open data file.\n");
                    return 1;
                }

                for (i = 0; i < k; i++) {
                    fprintf(data_file, "%d %f\n", i, values[i]);
                }

		fclose(data_file);

                sleep(1);
                k = 0;
		written = 1;
           }

	if(written){
		int count = 0;
		float value_read;
		float values_read[MAX_VALUES];
		char tmp[20];
		char line[MAX_LINE_LENGTH];

		FILE *file2 = fopen(DATA_FILE_NAME, "r");
                if (file2 == NULL) {
                    printf("Failed to open data file.\n");
                    return 1;
                }

		while(fgets(line, MAX_LINE_LENGTH, file2) != NULL && count < MAX_VALUES){
		    sscanf(line, "%*f %f", &value_read);
                    //printf("%f",  value_read);
		    values_read[count++] = value_read;

		    printf("%d %f\n", count-1, values_read[count-1]);
		    
		}
		fclose(file2);
	        printf("\n");	
	        if (first_run) {
	            printf("@autoscale\n"); 
		/* autoscale the first time through */
    		    first_run = 0;	        
		}
		printf("@redraw\n");
		printf("@sleep 0.1\n");   
		written = 0;
	}



    }
}
    pclose(fp);

    return 0;
}