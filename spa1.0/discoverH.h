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

int CheckHistory(int poolid, int clusterid, Base *History){

if(History->Headpool==NULL){
	return true;
}
else{
	PoolStruct *pool;
	pool=History->Headpool;
	while(pool!=NULL){

		if(pool->poolid==poolid){

			if(clusterid!=9999){
				if(pool->HeadCluster==NULL){
				return true;
				}
				else{
					ClusterStruct *cluster;
					cluster=pool->HeadCluster;
					while(cluster!=NULL){
						if(cluster->clusterid==clusterid){
							return false;
						}
						cluster=cluster->nextcluster;
					}
					return true;
				} 
			}
			return false;
		}

		pool=pool->nextpool;
	}
}
return true;
}
void RecordHistroy(int poolid, int clusterid, Base *History){
	 
	 if(clusterid==9999)
	 {	struct PoolStruct *p;
		 p= (struct PoolStruct *)malloc(sizeof(struct PoolStruct));
	     p->poolid=poolid;
	     p->nextpool= NULL;
	     if (History->Headpool==NULL) {
	     	History->Headpool=History->Tailpool=p;
	     	return;
	     }
	     	

	     else {
	          History->Tailpool->nextpool=p;
	          History->Tailpool=p;
	          return;
	     }
 	}
 	else{

 		PoolStruct *pool;
 		pool=History->Headpool;
		while(pool!=NULL){
		if(pool->poolid==poolid){
			struct ClusterStruct *c;
	        c= (struct ClusterStruct *)malloc(sizeof(struct ClusterStruct));
	 		c->parentpool= pool;
	 		c->clusterid=clusterid;
			 if (pool->HeadCluster==NULL) {
			 	pool->HeadCluster=pool->TailCluster=c;
			 	return;
			 }
		     	

		     else {
		          pool->TailCluster->nextcluster=c;
		          pool->TailCluster=c;
		          return;
		     }
		}
		pool = pool->nextpool;

 		}
	}
}
inline static int policycheck(List *Acqcores,int cpuid,char *policy){
CpuNode *temp;
temp=Acqcores->TailCpuNode;
int i,dum,mod;

if(!(strcmp(policy,"scatter")))
{
	if(Acqcores->HeadCpuNode==NULL){
		return true;
	}
	else
	{	mod=temp->cpuid%H_THREADS;
		if(mod==0)
		{
			dum = temp->cpuid-H_THREADS+1;
		}
		else{
			dum = temp->cpuid-mod+1;
		}

		for(i=0;i<H_THREADS;i++)
		{
			if(cpuid==dum){
				return false;
			}
			dum++;
		}
		return true;
	}
}
else
return true;
	
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
     	 // q->TailCpuNode->previous=q->TailCpuNode;
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
		node.next=NULL;
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
inline static ClusterStruct *TryLock(PoolStruct *pool,int req_cores,Base *History){
	struct ClusterStruct *temp1,*cluster=NULL;
	int cpucount=0;
	int32_t locked=true,update_lock=true;
	int check;

	temp1=pool->HeadCluster;
	if(pool->free_cores==0)
	{   while(SWAP(&pool->update_lock,update_lock));
		pool->application_count--;
		pool->update_lock=false;
		return NULL;
	}

	while(temp1!=NULL){
			check=CheckHistory(pool->poolid,temp1->clusterid,History);
			if(temp1->cpu_count > cpucount && check==true)
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
		cluster=TryLock(pool,req_cores,History);
	}
	else{
		RecordHistroy(pool->poolid,cluster->clusterid,History);
		return cluster;
	}
	
	return cluster;
}


//selects pool with less apps inside and select random cluster in that pool

inline static ClusterStruct *RandomCluster(int req_cores,char *policy,Base *History){
	struct PoolStruct *temp,*pool=NULL;
	struct ClusterStruct *cluster;
	int appcount=100;
	int32_t update_lock=true;
	int check=true;

	temp=Root->Headpool;
	while(SWAP(&temp->update_lock,update_lock));	//waiting for pool to get update
	while(temp!=NULL)
	{	check=CheckHistory(temp->poolid,9999,History);
		if(temp->application_count <=appcount && temp->free_cores>0 && check==true)
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
	RecordHistroy(pool->poolid,9999,History);

	cluster=TryLock(pool,req_cores,History);
	if(cluster==NULL)
	{
		cluster=RandomCluster(req_cores,policy,History);
	}

	return cluster;
}

//starts invading cores
 inline static void Discover(List *Acqcores,Application *app,Base *B){
 struct ClusterStruct *cluster;
 CpuNode node,*pnode;
 int32_t update_lock=true,pass;
 int count;
 Root=B;
 Base History={NULL,NULL};

	cluster=RandomCluster(app->cores,app->policy,&History);

	if(cluster==NULL)
	{
		return;
	}

	count=cluster->cpu_count;
	for(int i=0;i<app->cores;i++)
	{
		if(cluster->HeadCpuNode!=NULL&&count>0)
		{	
			count--;
			node=pop(cluster);
			
			pass=policycheck(Acqcores,node.cpuid,app->policy);
			if(!pass){
				pnode= (struct CpuNode *)malloc(sizeof(struct CpuNode));
				pnode->cpuid=node.cpuid;
     			pnode->parentcluster=node.parentcluster;
     			pnode->next=NULL;
            cluster->TailCpuNode->next=pnode;
            cluster->TailCpuNode=pnode;
			i--;

			}
			else{
			while(SWAP(&cluster->parentpool->update_lock,update_lock));
			cluster->cpu_count--;
			cluster->parentpool->free_cores--;
			cluster->parentpool->update_lock=false;
			push(Acqcores,node);
			}

		}
		
		else
		{

			cluster->locked=false;
			cluster=TryLock(cluster->parentpool,app->cores-i,&History);
			if(cluster==NULL){
				cluster=RandomCluster(app->cores-i,app->policy,&History);
				if(cluster==NULL)
				{
					return;
				}
			}
			count=cluster->cpu_count;
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