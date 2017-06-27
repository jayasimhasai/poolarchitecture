#ifndef _DISCOVER_H_
#define _DISCOVER_H_

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include <stdbool.h>
#include "pool.h"
#include "primitives.h"
#include "destroy.h"


//generate random cluster
inline static int SimRand();

inline static void push(List *q,CpuNode n);

inline static CpuNode pop(ClusterStruct *q);

//unlock cluster after invade
void Unlock(struct ClusterStruct *temp);

//trylock clusters inside pool with max number of cores
inline static ClusterStruct *TryLock(PoolStruct *pool,int req_cores);


//selects pool with less apps inside and select random cluster in that pool

inline static ClusterStruct *RandomCluster(int req_cores);

//starts invading cores
 void Discover(List *Acqcores,int req_cores,Base *B);

#endif