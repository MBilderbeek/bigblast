#include <stdio.h>
#include <math.h>


int main (int argc, char *argv[])
{
	int i;
	double angle;
	double factor=(double)atoi(argv[1]);
	
	for (i=0; i<32; i++)
	{
		angle = ((M_PI * 2.0) / 32.0) * (double) i;
		printf ("\t{ %d, %d } ,\n",  (int)(factor*cos(angle)),
					     (int)(factor*sin(angle)));
	}
	
}
