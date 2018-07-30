#ifndef _LIST_H_
#define _LIST_H_

struct nodeStruct
{
	int item;
	struct nodeStruct *next;
};

struct nodeStruct *List_createNode(int item);

void List_insertHead(struct nodeStruct **headRef, struct nodeStruct *node);

void List_insertTail(struct nodeStruct **headRef, struct nodeStruct *node);

void List_deleteHead(struct nodeStruct **headRef);

#endif