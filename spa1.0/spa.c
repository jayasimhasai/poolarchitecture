#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "conf.h" 
#include "pool.h"
#include "generate.h"
#include "invade.h"

#define MAX		50
int d1,d2,THREADS;
struct Base base;
//Global Variables
pthread_barrier_t barr;

struct Application
{	char *appname;
	int cores,id;	
}app;

void test(){
	struct PoolStruct *temp;
	struct ClusterStruct *temp1;
	struct CpuNode *temp2;
	temp=base.Headpool;
	while(temp!=NULL)
	{
		temp1=temp->HeadCluster;
			while(temp1!=NULL)
			{
				temp2=temp1->HeadCpuNode;
				while(temp2!=NULL)
				{
					printf("cpu %d in cluster %d and pool %d \n",temp2->cpuid,temp1->clusterid,temp->poolid);
					temp2=temp2->next;
				}
				printf("\n");
				temp1=temp1->nextcluster;
			}
			printf("\n");
			temp=temp->nextpool;
	}
}
inline static void Execute(void* Arg) {
    Application app = (Application) Arg;
    if (app.id == THREADS - 1)
        d1 = getTimeMillis();
    // Synchronization point
    int rc = pthread_barrier_wait(&barr);
    if (rc != 0 && rc != PTHREAD_BARRIER_SERIAL_THREAD) {
        printf("Could not wait on barrier\n");
        exit(-1);
    }
    Invade(app.cores);
    Impact();
    Retreat();

}

inline static void* EntryPoint(void* Arg) {
    Execute(Arg);
    return NULL;
}
inline static pthread_t StartThread(Application app) {
    void *Arg = (void*) app;
    pthread_t thread_p;
    int thread_id;

    pthread_attr_t my_attr;
    pthread_attr_init(&my_attr);
    thread_id = pthread_create(&thread_p, &my_attr, EntryPoint, Arg);

    return thread_p;
}

int main(int argc, char **argv) {
	pthread_t threads[MAX];
	int i,j;
	THREADS=argc-1;
	Setpools(&base);

	test();

	//Barrier initialization
	if (pthread_barrier_init(&barr,NULL,THREADS))
	{
		printf("Could not create the barrier\n");
        return -1;
	}

	for (i = 0,j=1; i < THREADS; i++,j++)
		{  app.appname=argv[2*j-1];
			app.cores=atoi(argv[2*j]);
			app.id=i;
        threads[i] = StartThread(app);
	}

    for (i = 0; i < THREADS; i++)
        pthread_join(threads[i], NULL);
    d2 = getTimeMillis();

    printf("time: %d\t", (int) (d2 - d1));

	return 0;
}