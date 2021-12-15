

#include "tLib.h"

#define FIRST_NODE		head.next
#define LAST_NODE		head.pre

void NodeInit(node_t * node)
{
	node->next = node;
	node->pre = node;
}

void ListInit(list_t * list)
{
	list->head.next = &(list->head);
	list->head.pre = &(list->head);
	list->node_cnt = 0;
}

uint32_t ListCount(list_t *list)
{
	return list->node_cnt;
}

node_t * ListFirst(list_t * list)
{
	node_t *node = (node_t *)0;
	
	if (list->node_cnt != 0)
	{
		node = list->head.next;
	}
	return node;
}

node_t * ListLast(list_t * list)
{
	node_t * node = (node_t *)0;
	
	if (list->node_cnt != 0)
	{
		node = list->head.pre;
	}
	return node;
}

node_t * ListPre(list_t * list, node_t * node)
{
	if (node->pre == node)
	{
		return (node_t *)0;
	}
	else
	{
		return node->pre;
	}
}

node_t * ListNext(list_t * list, node_t *node)
{
	if (node->next == node)
	{
		return (node_t *)0;
	}
	else
	{
		return node->next;
	}
}

void ListRemoveAll(list_t * list)
{
	uint32_t cnt = 0;
	node_t *next_node;
	
	next_node = list->head.next;
	for (cnt = list->node_cnt; cnt !=0; cnt--)
	{
		node_t *cur = next_node;
		next_node = next_node->next;
		
		cur->next = cur;
		cur->pre = cur;
	}
	
	list->head.next = &(list->head);
	list->head.pre = &(list->head);
	list->node_cnt = 0;
}

void ListAddFirst(list_t *list, node_t *node)
{
	node->pre = list->head.next->pre;
	node->next = list->head.next;
	
	list->head.next->pre = node;
	list->head.next = node;
	list->node_cnt++;
}

void ListAddLast(list_t *list, node_t *node)
{
	node->next = &(list->head);
	node->pre = list->head.pre;
	
	list->head.pre->next = node;
	list->head.pre = node;
	list->node_cnt++;
}

node_t * ListRemoveFirst(list_t *list)
{
	node_t *node = (node_t *)0;
	
	if (list->node_cnt !=0)
	{
		node = list->head.next;
		
		node->next->pre = &(list->head);
		list->head.next = node->next;
		list->node_cnt--;
	}
	return node;
}

void ListInsertAfter(list_t *list, node_t *nodeafter, node_t *node2insert)
{
	node2insert->pre = nodeafter;
	node2insert->next = nodeafter->next;
	
	nodeafter->next->pre = node2insert;
	nodeafter->next = node2insert;
	
	list->node_cnt++;
}

void ListRemove(list_t *list, node_t *node)
{
	node->pre->next = node->next;
	node->next->pre = node->pre;
	list->node_cnt--;
}



