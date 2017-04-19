#ifndef _PRIMITIVES_H_
#define _PRIMITIVES_H_

#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include <math.h>
#include <stdint.h>
#include <sys/timeb.h>
#include <malloc.h>


inline static double getTimeMillis(void) {
    double tseconds = 0.0;
    struct timeval mytime;
    gettimeofday(&mytime,(struct timezone*)0);
    tseconds = (double)(mytime.tv_sec + mytime.tv_usec*1.0e-6);
    return( tseconds );
   
}



#if defined(__GNUC__) && (__GNUC__*10000 + __GNUC_MINOR__*100) >= 40100
#    define __SWAP(A, B)               __sync_lock_test_and_set((long *)A, (long)B)

   
#elif defined(__GNUC__) && (defined(__amd64__) || defined(__x86_64__))
#    warning A newer version of GCC compiler is recommended!

inline static void *__SWAP(void *A, void *B) {
    int64_t *p = (int64_t *)A;

    asm volatile("lock;"
          "xchgq %0, %1"
          : "=r"(B), "=m"(*p)
          : "0"(B), "m"(*p)
          : "memory");
    return B;
}

#else
#    error Current machine architecture and compiler are not supported yet!
#endif


#define SWAP(A, B) _SWAP((void *)A, (void *)B)
inline static void *_SWAP(void *A, void *B) {
    return (void *)__SWAP(A, B);
}


#endif
