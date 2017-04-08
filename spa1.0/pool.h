#ifndef _POOL_H_
#define _POOL_H_

#include "conf.h"



typedef struct CpuNode
{	struct CpuNode *next;
	int cpuid;
}CpuNode;

typedef struct ClusterStruct
{	struct PoolStruct *parentpool;
	struct ClusterStruct *nextcluster;
	CpuNode *HeadCpuNode;
	CpuNode *TailCpuNode;
	int clusterid;
	int cpu_count;
	int32_t locked;

}ClusterStruct;

typedef struct PoolStruct
{	struct PoolStruct *nextpool;
	ClusterStruct *HeadCluster;
	ClusterStruct *TailCluster;
	int poolid;
	int application_count;
	int free_cores;
	int32_t update_lock;
}PoolStruct;

typedef struct Base
{	PoolStruct *Headpool;
	PoolStruct *Tailpool;
}Base;

typedef struct List
{	CpuNode *HeadCpuNode;
	CpuNode *TailCpuNode;
}List;

#endif