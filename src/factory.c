// 110121
// 110122

#include "factory.h"
#include "ut.h"
#include <string.h>

//////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
#  include <stdio.h>
void __factory_error() { printf("checked factory error\n"); }
#  define IS_DEBUG		True
#  define RETURN_FALSE	{ __factory_error(); return False; }
#  define RETURN_TRUE	{ return True; }
#else
#  define IS_DEBUG		False
#  define RETURN_FALSE	{ return False; }
#  define RETURN_TRUE	{ return True; }
#endif

//////////////////////////////////////////////////////////////////////////
Bool factory_add_block(factory_t *fac)
{
	if(fac && fac->block_top < fac->max_block_stack)
	{
		ushort i;
		void *block;

		// ����һ����
		block = ut_alloc(fac->product_len * fac->block_len);		// OK
		memset(block, 0, fac->product_len * fac->block_len);

		// �������Ŀ�
		fac->block_stack[fac->block_top] = block;
		fac->block_top++;

		// ���ָ��ջ�ռ䲻��
		if(fac->ptr_stack_len < fac->ptr_top + fac->block_len)
		{
			void *p;
			// ������ռ�
			fac->ptr_stack_len = fac->ptr_top + fac->block_len;

			p = ut_realloc(fac->ptr_stack, sizeof(void*) * fac->ptr_stack_len);	// OK
			if(p) fac->ptr_stack = (void**)p;
			else RETURN_FALSE;
		}

		// ����ָ��
		for(i = 0; i < fac->block_len; i++)
		{
			fac->ptr_stack[fac->ptr_top + i] = block;
			//block += fac->product_size;
			block = ((char*)block) + fac->product_len;
		}
		fac->ptr_top += fac->block_len;

		RETURN_TRUE;
	}
	RETURN_FALSE;
}

Bool factory_open(factory_t *fac, ushort product_len, ushort block_len, ushort max_block_stack)
{
	if(fac && product_len && block_len && max_block_stack)
	{
		memset(fac, 0, sizeof(factory_t));

		fac->product_len = product_len;
		fac->block_len = block_len;
		fac->max_block_stack = max_block_stack;

		// ����block list
		fac->block_stack = (void**)ut_alloc(sizeof(void*) * fac->max_block_stack);	// OK
		memset(fac->block_stack, 0, sizeof(void*) * fac->max_block_stack);

		// ���һ����
		return factory_add_block(fac);
	}
	RETURN_FALSE;
}

Bool factory_close(factory_t *fac)
{
	if(fac)
	{
		ushort i;

		// ����Ƿ����ڴ�й¶
		if(fac->ptr_top != fac->max_block_stack)
		{
			RETURN_FALSE;
		}

		for(i = 0; i < fac->block_top; i++)
		{
			ut_free(fac->block_stack[i]);
		}

		ut_free(fac->block_stack);
		ut_free(fac->ptr_stack);

		memset(fac, 0, sizeof(factory_t));

		RETURN_TRUE;
	}
	RETURN_FALSE;
}

void *_factory_yield(factory_t *fac)
{
	if(fac)
	{
		// ���ָ�붼������, ������¿�
		if(fac->ptr_top == 0)
			factory_add_block(fac);

		// ������п��õ�, ��ȡ��һ��
		if(fac->ptr_top != 0)
		{
			void *getted;
			getted = fac->ptr_stack[fac->ptr_top - 1];
			// ע��ȡ����Ҫ���
			fac->ptr_stack[fac->ptr_top - 1] = 0;
			fac->ptr_top--;
			return getted;
		}
	}
	return 0;
}

Bool _factory_recycle(factory_t *fac, void *p)
{
	if(fac && p)
	{
		ushort i;
		// ѭ�����п�
		for(i = 0; i < fac->block_top; i++)
		{
			// �õ���ǰ��
			void *block = fac->block_stack[i];
			void *block_end = ((char*)block) + fac->product_len * fac->block_len;

			// �ж�ָ���Ƿ����������֮��
			if(p >= block && p < block_end)
			{
				// ����ǰ���ָ��ջ�ڴ��Ƿ��㹻
				if(fac->ptr_top >= fac->ptr_stack_len)
				{
					void *tmp;

					// ����������
					fac->ptr_stack_len += fac->block_len;
					// ע����������Ϊ block_len * max_block_stack
					if(fac->ptr_stack_len > (uint)fac->block_len * (uint)fac->max_block_stack)
						fac->ptr_stack_len = fac->block_len * fac->max_block_stack;

					// ������ռ�
					tmp = ut_realloc(fac->ptr_stack, sizeof(void*) * fac->ptr_stack_len);	// OK
					// ����ɹ�������Ϊ��ǰָ��ջ
					if(tmp) fac->ptr_stack = (void**)tmp;
					else RETURN_FALSE;
				}

				// ����
				fac->ptr_stack[fac->ptr_top] = p;
				fac->ptr_top++;

				// ����Ƿ��зǷ��ڴ����
				if(fac->ptr_top > fac->ptr_stack_len)
				{
					RETURN_FALSE;
				}

				RETURN_TRUE;
			}
		}
	}
	RETURN_FALSE;
}
