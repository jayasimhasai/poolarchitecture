#ifndef _GENERATE_H
#define _GENERATE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "pool.h"
#include "conf.h"


int ApplyPolicy(int Pid, int Cid, int i);
void CreateCpu(struct ClusterStruct *C,int id);
void CreateCluster(struct PoolStruct *P,int Cid,int Pid);
void Createpool(struct Base *B,int id);
void Setpools(struct Base *base);

#endif
