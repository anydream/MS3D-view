// 101003 检查
// 101004 检查
// 101123 检查

#include "vid.h"
#include "viddebug.h"
#include "factory.h"
#include <string.h>

extern factory_t g_factory_vid_array;

#define vid_fac_arr_yield			((vid_array_t*)factory_yield(&g_factory_vid_array))
#define vid_fac_arr_recycle(_p)		(factory_recycle(&g_factory_vid_array, _p))

Bool vid_arr_create(vid_array_t **arr, VID_ARRAY_TYPE type, VID_USAGE usage, uint stride, const void *data, uint size)
{
	if(arr && size)
	{
		vid_array_t temp;

		// 此时可以填写结构中的数据
		{
			GLenum target = 0;
			GLuint id;

			// 判断需要创建顶点属性缓存还是索引缓存
			if(type == VID_VERTEX_ATTRIB) target = GL_ARRAY_BUFFER;
			else if(type == VID_INDEX) target = GL_ELEMENT_ARRAY_BUFFER;

			glGenBuffers(1, &id);
			if(!id) RETURN_FALSE;

			glBindBuffer(target, id);
			glBufferData(target, size, data, usage);
			CHECK_GL_ERR;

			if(IS_DEBUG) glBindBuffer(target, 0);

			temp.target = target;
			temp.array_id = id;
			temp.stride = stride;
		}

		// 创建结构, 可以从工厂中生产一个结构
		*arr = vid_fac_arr_yield;

		memmove(*arr, &temp, sizeof(vid_array_t));

		// 初始化引用计数为0
		refcount_init(*arr);

		// 添加一次引用
		reference(*arr);

		RETURN_TRUE;
	}
	RETURN_FALSE;
}

Bool vid_arr_release(vid_array_t **arr)
{
	if(arr)
	{
		// 如果引用为0
		if(reference_release(*arr))
		{
			// 释放成员数据
			glDeleteBuffers(1, &(*arr)->array_id);
			CHECK_GL_ERR;

			// 释放结构实体, 可以让工厂回收
			vid_fac_arr_recycle(*arr);
		}
		*arr = 0;
		RETURN_TRUE;
	}
	RETURN_FALSE;
}

Bool vid_arr_write(vid_array_t *arr, uint offset, const void *data, uint size)
{
	if(arr && arr->array_id && data && size)
	{
		glBindBuffer(arr->target, arr->array_id);
		glBufferSubData(arr->target, offset, size, data);
		CHECK_GL_ERR;
		if(IS_DEBUG) glBindBuffer(arr->target, 0);
		RETURN_TRUE;
	}
	RETURN_FALSE;
}

Bool vid_arr_as_indices(vid_array_t *arr, VID_VALUE_FORMAT fmt, uint offset)
{
	if(arr && arr->array_id && arr->target == GL_ELEMENT_ARRAY_BUFFER)
	{
		g_vid_system.indices_arr_id = arr->array_id;
		g_vid_system.indices_type = fmt;
		g_vid_system.indices_offset = offset;
		RETURN_TRUE;
	}
	RETURN_FALSE;
}