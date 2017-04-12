#ifndef _DISCOVER_H_
#define _DISCOVER_H_

#include <time.h>
#include "pool.h"
#include "primitives.h"
#include "destroy.h"

//Root to the pools
struct Base *Root;

//generate random cluster
inline static int SimRand(){
srand(time(NULL));   
int r = rand()%CLUSTER_COUNT; 
return r;
}

inline static void push(List *q,CpuNode n){
struct CpuNode *temp;						
     temp= (struct CpuNode *)malloc(sizeof(struct CpuNode));
     temp->cpuid=n.cpuid;
     temp->parentcluster=n.parentcluster;
     temp->next=NULL;
     if (q->HeadCpuNode==NULL) {
     	q->HeadCpuNode=temp;
     	q->TailCpuNode=temp;
     }
     else {
          q->TailCpuNode->next=temp;
          q->TailCpuNode=temp;
		}	
}

inline static CpuNode pop(ClusterStruct *q){
	struct CpuNode *temp,node;
	if(q->HeadCpuNode!=NULL)
	{
		node.cpuid=q->HeadCpuNode->cpuid;
		node.parentcluster=q->HeadCpuNode->parentcluster;
		temp=q->HeadCpuNode;
		q->HeadCpuNode=q->HeadCpuNode->next;
		free(temp);

	}
	
	return node;
}

//unlock cluster after invade
void Unlock(struct ClusterStruct *temp){
	int32_t locked=false;
	locked = (int32_t *)SWAP(&temp->locked,locked);
	return;
}

//trylock clusters inside pool with max number of cores
inline static ClusterStruct *TryLock(PoolStruct *pool,int req_cores){
	struct ClusterStruct *temp1,*cluster=NULL;
	int cpucount=0;
	int32_t locked=true,update_lock=true;

	temp1=pool->HeadCluster;
	if(pool->free_cores==0)
	{   while(SWAP(&pool->update_lock,update_lock));
		pool->application_count--;
		pool->update_lock=false;
		return NULL;
	}

	while(temp1!=NULL){
			if(temp1->cpu_count > cpucount)
			{
				cpucount=temp1->cpu_count;
				cluster=temp1;
			}
			temp1=temp1->nextcluster;
		}

	if(cluster==NULL)
	{
		return NULL;
	}

	if(SWAP(&cluster->locked,locked))		//trylock to other cluster
	{
		cluster=TryLock(pool,req_cores);
	}
	else
		return cluster;
	
	return cluster;
}


//selects pool with less apps inside and select random cluster in that pool

inline static ClusterStruct *RandomCluster(int req_cores){
	struct PoolStruct *temp,*pool=NULL;
	struct ClusterStruct *cluster;
	int appcount=100;
	int32_t update_lock=true;

	temp=Root->Headpool;
	while(SWAP(&temp->update_lock,update_lock));	//waiting for pool to get update
	while(temp!=NULL)
	{
		if(temp->application_count <=appcount && temp->free_cores>0)
		{
			appcount=temp->application_count;
			pool=temp;
		}
		temp=temp->nextpool;
	}

	if(pool==NULL)
	{	
		temp=Root->Headpool;
		temp->update_lock=false;
		return NULL;
	}

	pool->application_count++;
	temp=Root->Headpool;
	temp->update_lock=false;	//release update lock for other applications

	cluster=TryLock(pool,req_cores);
	if(cluster==NULL)
	{
		cluster=RandomCluster(req_cores);
	}
	return cluster;
}

//starts invading cores
 void Discover(List *Acqcores,int req_cores,Base *B){
 struct ClusterStruct *cluster;
 int32_t update_lock=true;
 Root=B;

	cluster=RandomCluster(req_cores);

	if(cluster==NULL)
	{
		return;
	}

	for(int i=0;i<req_cores;i++)
	{
		if(cluster->HeadCpuNode!=NULL)
		{	while(SWAP(&cluster->parentpool->update_lock,update_lock));
			cluster->cpu_count--;
			cluster->parentpool->free_cores--;
			cluster->parentpool->update_lock=false;
			push(Acqcores,pop(cluster));
		}
		
		else
		{

			cluster->locked=false;
			cluster=TryLock(cluster->parentpool,req_cores-i);
			if(cluster==NULL){
				cluster=RandomCluster(req_cores-i);
				if(cluster==NULL)
				{
					return;
				}
			}
			i--;
		}
	}
		
	cluster->locked=false;
	while(SWAP(&cluster->parentpool->update_lock,update_lock));
	cluster->parentpool->application_count--;
	cluster->parentpool->update_lock=false;

	return;
}







#endif