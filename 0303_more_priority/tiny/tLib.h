

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

#endif
