#ifndef _DISPATCH_H_
#define _DISPATCH_H_

#include <time.h>
#include <string.h>
#include "pool.h"


void Dispatch(List *Acqcores,char *appname){
CpuNode *temp;
char setcores[1024],buf[1024],args[1024];
int i=0,status,pid;
temp=Acqcores->HeadCpuNode;
	
	while(temp!=NULL)
	{
		sprintf(buf, "%d,", temp->cpuid);	
		strcat(setcores,buf);
		temp=temp->next;
		i++;	
	} 

	sprintf(args,"OMP_NUM_THREADS=%d KMP_AFFINITY=proclist=[%s],explicit",i,setcores);


	pid=fork();
	 if(pid!=0)
	 {
	 	execv("/bin/micnativeloadex", (char *[]){ "micnativeloadex",appname,"-e",args, NULL });
		 perror("execv");
		exit(0);

	 }
	else{
 	wait(&status);
 	printf("\ni am *****************%s***************\n",appname);
	}
	return;
}

#endif