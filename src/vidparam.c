// 101003 检查
// 101004 检查
// 101123 检查

#include "vid.h"
#include "viddebug.h"
#include "factory.h"
#include <string.h>

#pragma warning (disable: 4312)

extern factory_t g_factory_vid_param;

#define vid_fac_param_yield			((vid_param_t*)factory_yield(&g_factory_vid_param))
#define vid_fac_param_recycle(_p)	(factory_recycle(&g_factory_vid_param, _p))

Bool vid_param_bind(vid_param_t **param, vid_program_t *pro, const char *name)
{
	if(param && pro && pro->program && name)
	{
		vid_param_t temp;

		// 此时可以填写结构中的数据
		{
			// 获得对应名字的参数
			CGparameter parameter = cgGetNamedParameter(pro->program, name);
			CHECK_CG_ERR;
			if(!parameter) RETURN_FALSE;

			temp.program = reference(pro);
			temp.param = parameter;
		}

		// 创建结构, 可以从工厂中生产一个结构
		*param = vid_fac_param_yield;

		memmove(*param, &temp, sizeof(vid_param_t));

		// 初始化引用计数为0
		refcount_init(*param);

		// 添加一次引用
		reference(*param);

		RETURN_TRUE;
	}
	RETURN_FALSE;
}

Bool vid_param_release(vid_param_t **param)
{
	if(param)
	{
		Bool res = True;
		// 如果引用为0
		if(reference_release(*param))
		{
			// 释放成员数据
			res = vid_prog_release(&(*param)->program);

			// 释放结构实体, 可以让工厂回收
			vid_fac_param_recycle(*param);
		}
		*param = 0;
		if(res) RETURN_TRUE;
	}
	RETURN_FALSE;
}

Bool vid_param_enable_vertices(vid_param_t *param, Bool enable)
{
	// 参数合法性判断
	if(param && param->param)
	{
		// 开启和关闭顶点属性参数
		if(enable) cgGLEnableClientState(param->param);
		else cgGLDisableClientState(param->param);
		CHECK_CG_ERR;
		RETURN_TRUE;
	}
	RETURN_FALSE;
}

Bool vid_param_enable_texture(vid_param_t *param, Bool enable)
{
	// 参数合法性判断
	if(param && param->param)
	{
		// 开启和关闭纹理参数
		if(enable) cgGLEnableTextureParameter(param->param);
		else cgGLDisableTextureParameter(param->param);
		CHECK_CG_ERR;
		RETURN_TRUE;
	}
	RETURN_FALSE;
}

Bool vid_param_set_float(vid_param_t *param, float val)
{
	if(param && param->param)
	{
		// 设置一个浮点值
		cgSetParameter1f(param->param, val);
		CHECK_CG_ERR;
		RETURN_TRUE;
	}
	RETURN_FALSE;
}

Bool vid_param_set_float_2(vid_param_t *param, float val1, float val2)
{
	if(param && param->param)
	{
		// 设置两个浮点值
		cgSetParameter2f(param->param, val1, val2);
		CHECK_CG_ERR;
		RETURN_TRUE;
	}
	RETURN_FALSE;
}

Bool vid_param_set_float_3(vid_param_t *param, float val1, float val2, float val3)
{
	if(param && param->param)
	{
		// 设置三个浮点值
		cgSetParameter3f(param->param, val1, val2, val3);
		CHECK_CG_ERR;
		RETURN_TRUE;
	}
	RETURN_FALSE;
}

Bool vid_param_set_float_4(vid_param_t *param, float val1, float val2, float val3, float val4)
{
	if(param && param->param)
	{
		// 设置四个浮点值
		cgSetParameter4f(param->param, val1, val2, val3, val4);
		CHECK_CG_ERR;
		RETURN_TRUE;
	}
	RETURN_FALSE;
}

Bool vid_param_set_matrix(vid_param_t *param, const float *val)
{
	if(param && param->param && val)
	{
		// 设置一个矩阵
		cgSetMatrixParameterfr(param->param, val);
		CHECK_CG_ERR;
		RETURN_TRUE;
	}
	RETURN_FALSE;
}

Bool vid_param_set_float_array(vid_param_t *param, const float *val, int elements)
{
	if(param && param->param && val && elements)
	{
		// 设置多个浮点值
		cgSetParameterValuefr(param->param, elements, val);
		CHECK_CG_ERR;
		RETURN_TRUE;
	}
	RETURN_FALSE;
}

Bool vid_param_set_buffer(vid_param_t *param, vid_buffer_t *buf)
{
	if(param && param->param && param->program && param->program->program && buf && buf->buffer)
	{
		int idx = cgGetParameterBufferIndex(param->param);
		CHECK_CG_ERR;
		cgSetProgramBuffer(param->program->program, idx, buf->buffer);
		CHECK_CG_ERR;
		RETURN_TRUE;
	}
	RETURN_FALSE;
}

Bool vid_param_set_vertices(vid_param_t *param, vid_array_t *arr, VID_VALUE_FORMAT fmt, int fsize, uint offset)
{
	if(param && param->param && arr && arr->target == GL_ARRAY_BUFFER && arr->array_id)
	{
		glBindBuffer(GL_ARRAY_BUFFER, arr->array_id);
		cgGLSetParameterPointer(param->param, fsize, fmt, arr->stride, (void*)offset);
		CHECK_CG_ERR;
		if(IS_DEBUG) glBindBuffer(GL_ARRAY_BUFFER, 0);
		RETURN_TRUE;
	}
	RETURN_FALSE;
}

Bool vid_param_set_texture(vid_param_t *param, vid_texture_t *tex)
{
	if(param && param->param && tex && tex->tex_id)
	{
		cgGLSetTextureParameter(param->param, tex->tex_id);
		CHECK_CG_ERR;
		RETURN_TRUE;
	}
	RETURN_FALSE;
}