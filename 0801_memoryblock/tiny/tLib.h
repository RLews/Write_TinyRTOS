

#ifndef __TLIB_H
#define __TLIB_H

#include <stdint.h>

typedef struct _BIT_MAP_T
{
	uint32_t bitmap;
}bitmap_t;

void BitmapInit(bitmap_t *bitmap);
void BitmapSet(bitmap_t *bitmap, uint32_t pos);
void BitmapClear(bitmap_t *bitmap, uint32_t pos);
uint32_t BitmapGetFristSet(bitmap_t *bitmap);
uint32_t BitmapPosCount(void);

typedef struct _TNODE_T
{
	struct _TNODE_T * pre;
	struct _TNODE_T * next;
}node_t;

typedef struct _LIST_T
{
	node_t head;
	uint32_t node_cnt;
}list_t;

#define NODE_PARENT(node, parent, name)		(parent*)((uint32_t)node - (uint32_t)&((parent*)0)->name)

void NodeInit(node_t * node);
void ListInit(list_t * list);
uint32_t ListCount(list_t *list);
node_t * ListFirst(list_t * list);
node_t * ListLast(list_t * list);
node_t * ListPre(list_t * list, node_t * node);
node_t * ListNext(list_t * list, node_t *node);
void ListRemoveAll(list_t * list);
void ListAddFirst(list_t *list, node_t *node);
void ListAddLast(list_t *list, node_t *node);
node_t * ListRemoveFirst(list_t *list);
void ListInsertAfter(list_t *list, node_t *nodeafter, node_t *node2insert);
void ListRemove(list_t *list, node_t *node);


#endif
