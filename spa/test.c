#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "primitives.h"


void main(){

	int32_t A=true,B=false;
	int32_t *p;
	
	printf("%d %d %p %p\n",A,B,&A,&B);
	A=SWAP(&B,A);
    printf("%d %d %p %p %p\n",A,B,&A,&B,p);
}

