// 110124

#ifndef __Factory_h__
#define __Factory_h__

#include "types.h"

// 调试工厂内存泄漏
#ifdef _DEBUG
#  define FACTORY_DEBUG
#endif

#ifdef __cplusplus
extern "C"
{
#endif

// 一个链包含
// n个链实体内存块, malloc(sizeof(_type) * block_size)
// 一条用于存储n个链实体内存块指针的, 长度为n的指针列表
// 一条指针栈, 包含block_size个指针

// 指针工厂
typedef struct _factory
{
	ushort	product_len;		// 单个产品长度
	ushort	block_len;			// 块长度
	ushort	max_block_stack;	// 最大块指针栈长度

	uint	ptr_top;			// 指针栈顶
	uint	ptr_stack_len;		// 指针栈长度
	void	**ptr_stack;		// 指针栈

	ushort	block_top;			// 块指针栈顶
	void	**block_stack;		// 块指针栈
} factory_t;

// 添加一个块
//! 一般作为内部调用函数
Bool factory_add_block(factory_t *fac);

// 开启工厂
Bool factory_open(factory_t *fac, ushort product_len, ushort block_len, ushort max_block_stack);

// 关闭工厂
Bool factory_close(factory_t *fac);

#ifdef FACTORY_DEBUG
#  include "ut.h"
#  define factory_yield(_fac)			ut_alloc((_fac)->product_len)
#  define factory_recycle(_fac, _p)		(ut_free(_p), True)
#else

// 生产一个指针
void *_factory_yield(factory_t *fac);

// 回收一个指针
Bool _factory_recycle(factory_t *fac, void *p);

#  define factory_yield(_fac)			_factory_yield(_fac)
#  define factory_recycle(_fac, _p)		_factory_recycle(_fac, _p)

#endif


#ifdef __cplusplus
}
#endif

#endif