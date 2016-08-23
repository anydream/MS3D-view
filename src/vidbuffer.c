// 101023
// 101025 检查
// 101123 检查

#include "vid.h"
#include "viddebug.h"
#include "factory.h"
#include <string.h>

extern factory_t g_factory_vid_buffer;

#define vid_fac_buf_yield			((vid_buffer_t*)factory_yield(&g_factory_vid_buffer))
#define vid_fac_buf_recycle(_p)		(factory_recycle(&g_factory_vid_buffer, _p))

Bool vid_buf_create(vid_buffer_t **buf, VID_BUFFER_USAGE usage, const void *data, uint size)
{
	if(buf && g_vid_system.cg_context && size)
	{
		vid_buffer_t temp;

		// 此时可以填写结构中的数据
		{
			CGbuffer cgbuf = cgCreateBuffer(g_vid_system.cg_context, size, data, usage);
			CHECK_CG_ERR;
			if(!cgbuf) RETURN_FALSE;
			temp.buffer = cgbuf;
		}

		// 创建结构, 可以从工厂中生产一个结构
		*buf = vid_fac_buf_yield;

		memmove(*buf, &temp, sizeof(vid_buffer_t));

		// 初始化引用计数为0
		refcount_init(*buf);

		// 添加一次引用
		reference(*buf);

		RETURN_TRUE;
	}
	RETURN_FALSE;
}

Bool vid_buf_release(vid_buffer_t **buf)
{
	if(buf)
	{
		// 如果引用为0
		if(reference_release(*buf))
		{
			// 释放成员数据
			cgDestroyBuffer((*buf)->buffer);
			CHECK_CG_ERR;

			// 释放结构实体, 可以让工厂回收
			vid_fac_buf_recycle(*buf);
		}
		*buf = 0;
		RETURN_TRUE;
	}
	RETURN_FALSE;
}

Bool vid_buf_write(vid_buffer_t *buf, uint offset, const void *data, uint size)
{
	if(buf && buf->buffer && data && size)
	{
		cgSetBufferSubData(buf->buffer, offset, size, data);
		CHECK_CG_ERR;
		RETURN_TRUE;
	}
	RETURN_FALSE;
}