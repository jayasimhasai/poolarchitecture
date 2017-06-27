#ifndef _DESTROY_H_
#define _DESTROY_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include "pool.h"
#include "primitives.h"


void Pushback(struct CpuNode node);
inline static CpuNode Address(List *q);

void Destroy(struct List *Acqcores);

#endif