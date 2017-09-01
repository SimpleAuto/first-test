#ifndef _HASH_R_H
#define _HASH_R_H

#include <stdint.h>

static inline uint32_t hash_r5(const char* p)
{
	uint32_t h = 0;
	while(*p)
	{
		h = h * 11 + (*p << 4) + (*p >> 4);
		++p;
	}
	return h;
}

#endif // _HASH_R_H
