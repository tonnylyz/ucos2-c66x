#ifndef TYPES_H_
#define TYPES_H_

typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;

typedef u8 bool;
#define true 1
#define false 0

#define NULL ((void *)0)

static inline void _memcpy(void *dest, void *src, u32 n) {
    u32 i;
    for (i = 0; i < n; i++)
        ((char *) dest)[i] = ((char *) src)[i];
}

static inline void _memset(void *dest, u8 val, u32 n) {
    u32 i;
    for (i = 0; i < n; i++)
        ((char *) dest)[i] = val;
}

static inline int _strcmp(const char* s1, const char* s2) {
    while(*s1 && (*s1==*s2))
        s1++,s2++;
    return *(const unsigned char*)s1-*(const unsigned char*)s2;
}

#endif /* TYPES_H_ */
