// 101003
// 101012 检查
// 101031
// 101226 文件长改uint,封装内存操作
// 101228 MemoryLeak
// 110122 改进 ut_realloc

#ifndef __UT_h__
#define __UT_h__

#include "types.h"

#if !defined(_DEBUG)
#  include <malloc.h>
#elif defined (_MSC_VER)
#  define _CRTDBG_MAP_ALLOC
#  include <stdlib.h>
#  include <crtdbg.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

#define MAX(_a, _b) (((_a) > (_b)) ? (_a) : (_b))
#define MIN(_a, _b) (((_a) < (_b)) ? (_a) : (_b))

// 操作内存中的数据的辅助宏
#define AS_UCHAR(_p)			(((uchar*)_p) += sizeof(uchar), *(uchar*)(((uchar*)_p) - sizeof(uchar)))
#define AS_USHORT(_p)			(((uchar*)_p) += sizeof(ushort), *(ushort*)(((uchar*)_p) - sizeof(ushort)))
#define AS_UINT(_p)				(((uchar*)_p) += sizeof(uint), *(uint*)(((uchar*)_p) - sizeof(uint)))
#define AS_CHAR(_p)				(((uchar*)_p) += sizeof(char), *(char*)(((uchar*)_p) - sizeof(char)))
#define AS_INT(_p)				(((uchar*)_p) += sizeof(int), *(int*)(((uchar*)_p) - sizeof(int)))
#define AS_FLOAT(_p)			(((uchar*)_p) += sizeof(float), *(float*)(((uchar*)_p) - sizeof(float)))

#define READ_CHAR3(_o, _p)		((_o)[0] = AS_CHAR(_p), (_o)[1] = AS_CHAR(_p), (_o)[2] = AS_CHAR(_p))
#define READ_UCHAR3(_o, _p)		((_o)[0] = AS_UCHAR(_p), (_o)[1] = AS_UCHAR(_p), (_o)[2] = AS_UCHAR(_p))
#define READ_USHORT3(_o, _p)	((_o)[0] = AS_USHORT(_p), (_o)[1] = AS_USHORT(_p), (_o)[2] = AS_USHORT(_p))
#define READ_FLOAT3(_o, _p)		((_o)[0] = AS_FLOAT(_p), (_o)[1] = AS_FLOAT(_p), (_o)[2] = AS_FLOAT(_p))
#define READ_FLOAT4(_o, _p)		((_o)[0] = AS_FLOAT(_p), (_o)[1] = AS_FLOAT(_p), (_o)[2] = AS_FLOAT(_p), (_o)[3] = AS_FLOAT(_p))

#define WRITE_CHAR3(_i, _p)		(AS_CHAR(_p) = (_i)[0], AS_CHAR(_p) = (_i)[1], AS_CHAR(_p) = (_i)[2])
#define WRITE_UCHAR3(_i, _p)	(AS_UCHAR(_p) = (_i)[0], AS_UCHAR(_p) = (_i)[1], AS_UCHAR(_p) = (_i)[2])
#define WRITE_USHORT3(_i, _p)	(AS_USHORT(_p) = (_i)[0], AS_USHORT(_p) = (_i)[1], AS_USHORT(_p) = (_i)[2])
#define WRITE_FLOAT3(_i, _p)	(AS_FLOAT(_p) = (_i)[0], AS_FLOAT(_p) = (_i)[1], AS_FLOAT(_p) = (_i)[2])
#define WRITE_FLOAT4(_i, _p)	(AS_FLOAT(_p) = (_i)[0], AS_FLOAT(_p) = (_i)[1], AS_FLOAT(_p) = (_i)[2], AS_FLOAT(_p) = (_i)[3])

// 所有的内存操作都必须使用以下方法
#define ut_alloc(_sz)			((_sz) ? malloc(_sz) : 0)
#define ut_realloc(_p, _sz)		((_p) ? ((_sz) ? realloc(_p, _sz) : _p) : ((_sz) ? malloc(_sz) : 0))
void	ut_free(void *p);

void	*ut_file_read(const wchar_t *file, uint *len);
void	ut_file_free(void *buf);

void	ut_srand(uint seed);
int		ut_rand();
int		ut_random(int minimum, int maximum);

#ifdef __cplusplus
}
#endif

#endif