// 110124

#ifndef __Factory_h__
#define __Factory_h__

#include "types.h"

// ���Թ����ڴ�й©
#ifdef _DEBUG
#  define FACTORY_DEBUG
#endif

#ifdef __cplusplus
extern "C"
{
#endif

// һ��������
// n����ʵ���ڴ��, malloc(sizeof(_type) * block_size)
// һ�����ڴ洢n����ʵ���ڴ��ָ���, ����Ϊn��ָ���б�
// һ��ָ��ջ, ����block_size��ָ��

// ָ�빤��
typedef struct _factory
{
	ushort	product_len;		// ������Ʒ����
	ushort	block_len;			// �鳤��
	ushort	max_block_stack;	// ����ָ��ջ����

	uint	ptr_top;			// ָ��ջ��
	uint	ptr_stack_len;		// ָ��ջ����
	void	**ptr_stack;		// ָ��ջ

	ushort	block_top;			// ��ָ��ջ��
	void	**block_stack;		// ��ָ��ջ
} factory_t;

// ���һ����
//! һ����Ϊ�ڲ����ú���
Bool factory_add_block(factory_t *fac);

// ��������
Bool factory_open(factory_t *fac, ushort product_len, ushort block_len, ushort max_block_stack);

// �رչ���
Bool factory_close(factory_t *fac);

#ifdef FACTORY_DEBUG
#  include "ut.h"
#  define factory_yield(_fac)			ut_alloc((_fac)->product_len)
#  define factory_recycle(_fac, _p)		(ut_free(_p), True)
#else

// ����һ��ָ��
void *_factory_yield(factory_t *fac);

// ����һ��ָ��
Bool _factory_recycle(factory_t *fac, void *p);

#  define factory_yield(_fac)			_factory_yield(_fac)
#  define factory_recycle(_fac, _p)		_factory_recycle(_fac, _p)

#endif


#ifdef __cplusplus
}
#endif

#endif