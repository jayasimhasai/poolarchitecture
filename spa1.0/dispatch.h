#ifndef _DISPATCH_H_
#define _DISPATCH_H_

#include <time.h>
#include <string.h>
#include "pool.h"


void Dispatch(List *Acqcores,char *appname){
CpuNode *temp;
char setcores[1024],buf[1024],args[1024];
int i=0;
temp=Acqcores->HeadCpuNode;
	while(temp!=NULL)
	{
		temp->cpuid=temp->cpuid+'0';
		sprintf(buf, "%d,", temp->cpuid);	
		strcat(setcores,buf);
		temp=temp->next;
		i++;
		
	} 
printf("%s %s",setcores,appname);
sprintf(args,"./%s -e \"OMP_NUM_THREADS=%d KMP_AFFINITY=proclist=[%s],explicit\"",appname,i,setcores);
printf("%s",args);

// if(fork()!=0)
// {
// 	execv(micnativelodex, args);
// }

}

#endif