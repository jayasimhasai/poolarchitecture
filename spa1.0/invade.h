#ifndef _INVADE_H_
#define _INVADE_H_

#include <time.h>
#include "pool.h"
#include "primitives.h"


struct Base *Root;
inline static int SimRand(){
srand(time(NULL));   
int r = rand()%CLUSTER_COUNT; 
return r;
}
inline static void push(struct ClusterStruct *q,int i){
struct CpuNode *temp;						

     temp= (struct CpuNode *)malloc(sizeof(struct CpuNode));
     temp->cpuid=i;
     temp->next=NULL;
     if (q->HeadCpuNode==NULL) q->HeadCpuNode=q->TailCpuNode=temp;
     else {
          q->TailCpuNode->next=temp;
          q->TailCpuNode=temp;
		}	
}

inline static int pop(struct ClusterStruct *q){
	int i;
	struct CpuNode *temp;
	if(q->HeadCpuNode!=NULL)
	{
		i=q->HeadCpuNode->cpuid;
		temp=q->HeadCpuNode;
		q->HeadCpuNode=q->HeadCpuNode->next;
		free(temp);

	}
	
	return i;
}

void Unlock(struct ClusterStruct *temp){
int32_t locked=false;
locked = (int32_t)SWAP(&temp->locked,locked);
return;
}

ClusterStruct *TryLock(struct PoolStruct *temp){
	struct ClusterStruct *temp1;
	int acqcluster=1,clusterid;
	int32_t locked=true;
	clusterid=SimRand();
	temp1=temp->HeadCluster;
	while(temp1->clusterid!=clusterid){
			temp1=temp1->nextcluster;
		}

	locked = (int32_t)SWAP(&temp1->locked,locked);
	if(locked)
		temp1=TryLock(temp);
	else
		return temp1;
	
	return temp1;
}

ClusterStruct *RandomCluster(int req_cores){
	struct PoolStruct *temp;
	struct ClusterStruct *temp1;
	int appcount=0,freecores=0,poolid;
	temp=Root->Headpool;
	while(temp!=NULL){
		if(temp->free_cores >freecores)
		{
			freecores=temp->free_cores;
			poolid=temp->poolid;
		}
		temp=temp->nextpool;
	}
	temp=Root->Headpool;
	while(temp->poolid!=poolid)
		temp=temp->nextpool;
	temp1=TryLock(temp);
	return temp1;
}

void Invade(struct List *Acqcores,int req_cores,struct Base *B){
 struct ClusterStruct *temp1;
 Root=B;
	temp1=RandomCluster(req_cores);
	for(int i=0;i<req_cores;i++)
	{
		if(temp1->HeadCpuNode!=NULL)
		{
			push(Acqcores,pop(temp1));
		}
		
		else{
			
			Unlock(temp1);
			temp1=TryLock(temp1->parentpool);
			i--;
		}
	}
		
	Unlock(temp1);

}







#endif