#include "destroy.h"

void Pushback(struct CpuNode node){
	struct ClusterStruct *parentcluster;
	struct PoolStruct *parentpool;
	struct CpuNode *temp;
	int32_t locked=true,update_lock=true;

	temp= (struct CpuNode *)malloc(sizeof(struct CpuNode));
     temp->cpuid=node.cpuid;
     temp->parentcluster=node.parentcluster;
     temp->next=NULL;
	parentcluster=node.parentcluster;
	
	while(SWAP(&parentcluster->locked,locked));
	
	if (parentcluster->HeadCpuNode==NULL) {
		parentcluster->HeadCpuNode=parentcluster->TailCpuNode=temp;
	}
    else {
          parentcluster->TailCpuNode->next=temp;
          parentcluster->TailCpuNode=temp;
	}
		
		parentcluster->cpu_count++;
		parentcluster->locked=false;
		parentpool=parentcluster->parentpool;
		
		while(SWAP(&parentpool->update_lock,update_lock));
		
		parentpool->free_cores++;
		parentpool->update_lock=false;

}
inline static CpuNode Address(List *q){
	struct CpuNode *temp,node;
	if(q->HeadCpuNode!=NULL)
	{
		node.cpuid=q->HeadCpuNode->cpuid;
		node.parentcluster=q->HeadCpuNode->parentcluster;
		temp=q->HeadCpuNode;
		q->HeadCpuNode=q->HeadCpuNode->next;
		free(temp);

	}
	
	return node;
}

void Destroy(struct List *Acqcores){
struct CpuNode node;
	
	while(Acqcores->HeadCpuNode!=NULL){
		node=Address(Acqcores);
		Pushback(node);
	}
}