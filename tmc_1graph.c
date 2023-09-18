/*
 * test of -pipe option
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifndef M_PI
#  define M_PI 3.14
#endif

int main(int argc, char **argv)
{
    int i, j;
    double t;


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

    for (j = 20; j > 0; j--) {
	printf("@with g0\n");               /* reset the current graph to graph 0 */
	printf("@kill s0\n");               /* make sure s0 is available
                                             * (data will be read into this set)
                                             */

	/*
	 * write out a set
	 */
	for (i = 0; i < 101; i++) {
	    t = 8.0 * i / (1.0 * j) * M_PI;
	    printf("%d %f\n", i, cos(t) + sin(2.0 * t) + cos(t / 2.0) + sin(4 * t) + cos(t / 4.0));
	}

	printf("&\n");		/* end of set marker */

	if (j == 20) {
	    printf("@autoscale\n"); 		/* autoscale the first time through */
	}
	printf("@redraw\n");
	printf("@sleep 0.1\n");                   /* let the graph sit for a sec */
    }
    
    exit(0);
}
