#ifndef __MYFLOAT_TYPES_H__
#define __MYFLOAT_TYPES_H__

#ifdef __cplusplus
extern "C" {
#endif


#include <stdint.h>
#include <stdbool.h>


#define MYFLOAT_LUTYPE 	uint64_t
#define MYFLOAT_UTYPE 	uint32_t
#define MYFLOAT_STYPE     int32_t
#define MYFLOAT_WIDTH     32
#define MYFLOAT_ZERO      ((MYFLOAT_UTYPE)0x00000000)
#define MYFLOAT_NAR       ((MYFLOAT_UTYPE)0x80000000)
#define MYFLOAT_MSB       ((MYFLOAT_UTYPE)0x80000000)
#define MYFLOAT_MASK      ((MYFLOAT_UTYPE)0xFFFFFFFF)



#ifdef __cplusplus
}
#endif

#endif