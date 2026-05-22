
#ifndef __BSDIFF_H__
#define  __BSDIFF_H__

#include <stdint.h>
#include <stdlib.h>

void bspatch(const uint8_t* old, int32_t oldsize, const uint8_t* patch, int32_t patchsize, uint8_t* new, int32_t newsize);

#endif /* BSDIFF_H_ */
