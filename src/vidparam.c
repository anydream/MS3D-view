// 101003 ���
// 101004 ���
// 101123 ���

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

		// ��ʱ������д�ṹ�е�����
		{
			// ��ö�Ӧ���ֵĲ���
			CGparameter parameter = cgGetNamedParameter(pro->program, name);
			CHECK_CG_ERR;
			if(!parameter) RETURN_FALSE;

			temp.program = reference(pro);
			temp.param = parameter;
		}

		// �����ṹ, ���Դӹ���������һ���ṹ
		*param = vid_fac_param_yield;

		memmove(*param, &temp, sizeof(vid_param_t));

		// ��ʼ�����ü���Ϊ0
		refcount_init(*param);

		// ���һ������
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
		// �������Ϊ0
		if(reference_release(*param))
		{
			// �ͷų�Ա����
			res = vid_prog_release(&(*param)->program);

			// �ͷŽṹʵ��, �����ù�������
			vid_fac_param_recycle(*param);
		}
		*param = 0;
		if(res) RETURN_TRUE;
	}
	RETURN_FALSE;
}

Bool vid_param_enable_vertices(vid_param_t *param, Bool enable)
{
	// �����Ϸ����ж�
	if(param && param->param)
	{
		// �����͹رն������Բ���
		if(enable) cgGLEnableClientState(param->param);
		else cgGLDisableClientState(param->param);
		CHECK_CG_ERR;
		RETURN_TRUE;
	}
	RETURN_FALSE;
}

Bool vid_param_enable_texture(vid_param_t *param, Bool enable)
{
	// �����Ϸ����ж�
	if(param && param->param)
	{
		// �����͹ر��������
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
		// ����һ������ֵ
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
		// ������������ֵ
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
		// ������������ֵ
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
		// �����ĸ�����ֵ
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
		// ����һ������
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
		// ���ö������ֵ
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