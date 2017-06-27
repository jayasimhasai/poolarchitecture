#ifndef _CONF_H_

#define _CONF_H_

//Definition: USE_CPUS
// --------------------
// Define the number of processing physical cores for system with pool architechture

#ifndef USE_CPUS
#	define USE_CPUS				60
#endif

//Definition: H_THREADS
// --------------------
//Defines number of hardware threads per core
// system offers or the maximum number of cores depends on USE_CPUS and H_THREADS that you like to use.
#ifndef H_THREADS
#	define H_THREADS			4
#endif

//Definition: POOL_COUNT
//-----------------------
//Define the number of pools in the architecture
#ifndef POOL_COUNT
#	define POOL_COUNT			3
#endif

//Definition: CLUSTER_COUNT
//-------------------------
//Define the number of clusters inside each pool
#ifndef CLUSTER_COUNT
#	define CLUSTER_COUNT		4
#endif

//Definition: POLICY
//------------------
//Define the policy implemented in the pools
//supported policies SCATTER, COMPACT, EXPLICIT
#ifndef POLICY
#	define POLICY 				"SCATTER"
#endif

#endif

