#ifndef _GENERATE_H
#define _GENERATE_H

#include "pool.h"
#include "conf.h"

#define FREE_CORES		(USE_CPUS*H_THREADS/POOL_COUNT)
#define CPU_COUNT 		(FREE_CORES/CLUSTER_COUNT)


int ApplyPolicy(int Pid, int Cid, int i)
{	int cpuid,temp;
	if(POLICY=="SCATTER")
	{
		temp=Pid*FREE_CORES+Cid*CPU_COUNT+i;
		cpuid=(temp*H_THREADS)%(USE_CPUS*H_THREADS)+((temp*H_THREADS)/(USE_CPUS*H_THREADS))+1;
	}
	else if(POLICY=="COMPACT")
	{
		cpuid=Pid*FREE_CORES+Cid*CPU_COUNT+i+1;
	}
	return cpuid;

}
void CreateCpu(struct ClusterStruct *C,int id)
{
	struct CpuNode *n;
	n= (struct CpuNode *)malloc(sizeof(struct CpuNode));
	n->cpuid = id;
	n->next= NULL;
	if (C->HeadCpuNode==NULL) 
     	C->HeadCpuNode = C->TailCpuNode = n;

     else {
          C->TailCpuNode->next=n;
          C->TailCpuNode=n;
     }
}

void CreateCluster(struct PoolStruct *P,int Cid,int Pid)
{
	struct ClusterStruct *c;
	 int cpuid;
	 c= (struct ClusterStruct *)malloc(sizeof(struct ClusterStruct));
	 c->clusterid=Cid;
	 c->cpu_count=CPU_COUNT;
	 c->locked= false;
	 for(int i=0;i<CPU_COUNT;i++)
	{
		cpuid=ApplyPolicy(Pid,Cid,i);
		CreateCpu(c,cpuid);
	}
	 if (P->HeadCluster==NULL) 
     	P->HeadCluster=P->TailCluster=c;

     else {
          P->TailCluster->nextcluster=c;
          P->TailCluster=c;
     }
}
void Createpool(struct Base *B,int id)
{
	struct PoolStruct *p;						

     p= (struct PoolStruct *)malloc(sizeof(struct PoolStruct));
     p->poolid=id;
     p->application_count=0;	
     p->free_cores=FREE_CORES;
     p->update_lock= false;
     p->nextpool= NULL;
     for(int i=0;i<CLUSTER_COUNT;i++)
	{
		CreateCluster(p,i,id);
	}
     if (B->Headpool==NULL) 
     	B->Headpool=B->Tailpool=p;

     else {
          B->Tailpool->nextpool=p;
          B->Tailpool=p;
     }

}
void Setpools(struct Base *base){

	for(int i=0;i<POOL_COUNT;i++)
	{
		Createpool(base,i);
		//printf("%d   %d 	%d 	%d 	%d ",USE_CPUS,POOL_COUNT,CLUSTER_COUNT,FREE_CORES,CPU_COUNT);
	}
}

#endif
