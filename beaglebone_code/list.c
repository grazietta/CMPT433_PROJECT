#include <stdio.h>
#include "list.h"
#include <string.h>
#include <stdlib.h>

struct nodeStruct *List_createNode(int item)
{
    struct nodeStruct *temp = malloc(sizeof(struct nodeStruct));
    temp->item = item;
    temp->next = NULL;
    return temp;
}

void List_insertHead(struct nodeStruct **headRef, struct nodeStruct *node)
{
    node->next = *headRef;
    *headRef = node;
}

void List_insertTail(struct nodeStruct **headRef, struct nodeStruct *node)
{
    struct nodeStruct *temp = *headRef;
    if (List_countNodes(*headRef) == 0)
    {
        node->next = *headRef;
        *headRef = node;
        return;
    }
    while (temp->next != NULL)
    {
        temp = temp->next;
    }
    temp->next = node;
    node->next = NULL;
}

int List_countNodes(struct nodeStruct *head)
{
    struct nodeStruct *temp = head;
    int count = 0;
    if (head == NULL)
    {
        return 0;
    }
    while (temp != NULL)
    {
        temp = temp->next;
        count++;
    }
    return count;
}

void List_deleteHead(struct nodeStruct **headRef)
{
    struct nodeStruct *temp = *headRef;
    *headRef = temp->next;
    free(temp);
}
