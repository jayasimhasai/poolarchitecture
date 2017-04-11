#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "conf.h" 
#include "pool.h"
#include "generate.h"
#include "discover.h"
#include "destroy.h"
#include "primitives.h"

#define MAX		50
int d1,d2,THREADS;
struct Base base;
clock_t start, end;
double cpu_time_used;
//Global Variables
pthread_barrier_t barr;

struct Application
{	char *appname;
	int cores,id;	
};

void test(){
	struct PoolStruct *temp;
	struct ClusterStruct *temp1;
	struct CpuNode *temp2;
	temp=base.Headpool;
	while(temp!=NULL)
	{
		temp1=temp->HeadCluster;
			printf("\n pool details freecpu's  %d application count %d lock status %d",temp->free_cores,temp->application_count,temp->update_lock);
			while(temp1!=NULL)
			{
				temp2=temp1->HeadCpuNode;
				printf("\n cluster details cpu_count %d lock status %d",temp1->cpu_count,temp1->locked);
				while(temp2!=NULL)
				{
					printf("\n cpu %d in cluster %d and pool %d",temp2->cpuid,temp1->clusterid,temp->poolid);
					temp2=temp2->next;
				}
				printf("\n");
				temp1=temp1->nextcluster;
			}
			printf("\n");
			temp=temp->nextpool;
	}
}
inline static void *Execute(void* Arg) {
    struct Application *app = (struct Application *) Arg;
    struct List Acq_cores;
    struct CpuNode nodedetails;
    if (app->id == THREADS - 1)
        start = clock();
    // Synchronization point
    int rc = pthread_barrier_wait(&barr);
    if (rc != 0 && rc != PTHREAD_BARRIER_SERIAL_THREAD) {
        printf("Could not wait on barrier\n");
        exit(-1);
    }
    Discover(&Acq_cores,app->cores,&base);

    //for(int i=0;i<app->cores;i++){
    	//nodedetails=pop(&Acq_cores);
    	//printf("\n application %s, required cores %d core details core id %d, cluster id %d, pool id %d",app->appname,app->cores,nodedetails.cpuid,nodedetails.parentcluster->clusterid,nodedetails.parentcluster->parentpool->poolid);
   // }
    //Dispatch();
    Destroy(&Acq_cores,app->cores);

}

inline static pthread_t StartThread(struct Application *app) {
    void *Arg = (void *)app;
    pthread_t thread_p;
    int thread_id;
    pthread_attr_t my_attr;
    pthread_attr_init(&my_attr);
    thread_id = pthread_create(&thread_p, &my_attr, Execute, Arg);

    return thread_p;
}

int main(int argc, char **argv) {
	pthread_t threads[MAX];
	int i,j;
	struct Application *app;
	THREADS=(argc-1)/2;
	Setpools(&base);

	//test();

	//Barrier initialization
	if (pthread_barrier_init(&barr,NULL,THREADS))
	{
		printf("Could not create the barrier\n");
        return -1;
	}

	for (i = 0,j=1; i < THREADS; i++,j++)
		{  app = malloc(sizeof(struct Application));
			app->appname=argv[2*j-1];
			app->cores=atoi(argv[2*j]);
			app->id=i;
        threads[i] = StartThread(app);
	}

    for (i = 0; i < THREADS; i++)
        pthread_join(threads[i], NULL);
    d2 = getTimeMillis();
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("\nCPU_CYCLES: %d\t", cpu_time_used);
    printf("\ntime: %d\t", (int) (d2 - d1));
   //test();

	return 0;
}