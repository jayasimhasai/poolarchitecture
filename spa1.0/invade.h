#ifndef _INVADE_H_
#define _INVADE_H_

#include <time.h>
#include "pool.h"
#include "primitives.h"

//Root to the pools
struct Base *Root;

//generate random cluster
inline static int SimRand(){
srand(time(NULL));   
int r = rand()%CLUSTER_COUNT; 
return r;
}

inline static void push(struct ClusterStruct *q,CpuNode n){
struct CpuNode *temp;						

     temp= (struct CpuNode *)malloc(sizeof(struct CpuNode));
     temp->cpuid=n.cpuid;
     temp->parentcluster=n.parentcluster;
     temp->next=NULL;
     if (q->HeadCpuNode==NULL) q->HeadCpuNode=q->TailCpuNode=temp;
     else {
          q->TailCpuNode->next=temp;
          q->TailCpuNode=temp;
		}	
}

inline static CpuNode pop(struct ClusterStruct *q){
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

//trylock clusters inside pool 
inline static ClusterStruct *TryLock(struct PoolStruct *temp,int req_cores){
	struct ClusterStruct *temp1,*cluster;
	int acqcluster=1,clusterid,cpucount=0;
	int32_t locked=true;
	//clusterid=SimRand();
	temp1=temp->HeadCluster;
	if(temp->free_cores==0)
	{
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

	locked = (int32_t *)SWAP(&cluster->locked,locked);
	if(locked)
		cluster=TryLock(temp,req_cores);
	else
		return cluster;
	
	return cluster;
}


//selects pool with less apps inside and select random cluster in that pool

inline static ClusterStruct *RandomCluster(int req_cores){
	struct PoolStruct *temp,*pool;
	struct ClusterStruct *temp1;
	int appcount=0,freecores=0,poolid;
	int32_t update_lock=true;
	temp=Root->Headpool;
	while(SWAP(&temp->update_lock,update_lock));
	while(temp!=NULL){
		if(temp->application_count <=appcount && temp->free_cores>0)
		{
			appcount=temp->application_count;
			pool=temp;
		}
		temp=temp->nextpool;
	}
	pool->application_count++;
	temp=Root->Headpool;
	temp->update_lock=false;
	temp1=TryLock(pool,req_cores);
	return temp1;
}

//starts invading cores
 void Invade(struct List *Acqcores,int req_cores,struct Base *B){
 struct ClusterStruct *temp1;
 Root=B;
	temp1=RandomCluster(req_cores);
	for(int i=0;i<req_cores;i++)
	{
		if(temp1->HeadCpuNode!=NULL)
		{	
			temp1->cpu_count--;
			temp1->parentpool->free_cores--;
			push(Acqcores,pop(temp1));
		}
		
		else{

			Unlock(temp1);
			temp1=TryLock(temp1->parentpool,req_cores-i);
			if(temp1==NULL){
				temp1=RandomCluster(req_cores-i);
			}
			i--;
		}
	}
		
	Unlock(temp1);
	temp1->parentpool->application_count--;

}







#endif