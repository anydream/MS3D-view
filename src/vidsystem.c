// 101002 ���

#include "vid.h"
#include "viddebug.h"
#include "factory.h"

#pragma warning (disable: 4312)

//////////////////////////////////////////////////////////////////////////
// ȫ��ֻ��һ�� vid_system_t
vid_system_t g_vid_system = { 0, 0, 0, 0 };

// vid ��Ⱦ�ṹ�Ĺ���
factory_t g_factory_vid_texture;
factory_t g_factory_vid_rendertar;
factory_t g_factory_vid_array;
factory_t g_factory_vid_buffer;
factory_t g_factory_vid_program;
factory_t g_factory_vid_param;

//////////////////////////////////////////////////////////////////////////
// ���� vid �ṹ����
Bool vid_factory_open()
{
	Bool res;
	res = factory_open(&g_factory_vid_texture,		sizeof(vid_texture_t),		64, 64);
	res = factory_open(&g_factory_vid_rendertar,	sizeof(vid_rendertar_t),	64, 64) && res;
	res = factory_open(&g_factory_vid_array,		sizeof(vid_array_t),		64, 64) && res;
	res = factory_open(&g_factory_vid_buffer,		sizeof(vid_buffer_t),		64, 64) && res;
	res = factory_open(&g_factory_vid_program,		sizeof(vid_program_t),		64, 64) && res;
	res = factory_open(&g_factory_vid_param,		sizeof(vid_param_t),		64, 64) && res;

	return res;
}

// �ر� vid �ṹ����
Bool vid_factory_close()
{
	Bool res;
	res = factory_close(&g_factory_vid_texture);
	res = factory_close(&g_factory_vid_rendertar) && res;
	res = factory_close(&g_factory_vid_array) && res;
	res = factory_close(&g_factory_vid_buffer) && res;
	res = factory_close(&g_factory_vid_program) && res;
	res = factory_close(&g_factory_vid_param) && res;

	return res;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
Bool vid_sys_create()
{
	CGcontext context;

	if(!vid_factory_open()) RETURN_FALSE;
	// ��ʼ�� GL
	{
		if(IS_DEBUG) cgGLSetDebugMode(True);
		else cgGLSetDebugMode(False);

		// ����Ĭ����Ȳ���
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);

		// �������
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// ͸������
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		// ���� Mipmap ����
		glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);
	}

	// ���� CG ����
	context = cgCreateContext();
	CHECK_CG_ERR;
	if(!context) RETURN_FALSE;

	g_vid_system.cg_context = context;
	g_vid_system.indices_arr_id = 0;
	g_vid_system.indices_type = 0;
	g_vid_system.indices_offset = 0;

	RETURN_TRUE;
}

Bool vid_sys_destroy()
{
	if(g_vid_system.cg_context)
	{
		cgDestroyContext(g_vid_system.cg_context);
		CHECK_CG_ERR;

		if(!vid_factory_close()) RETURN_FALSE;
		RETURN_TRUE;
	}
	RETURN_FALSE;
}

Bool vid_sys_deferred_param(Bool enable)
{
	if(g_vid_system.cg_context)
	{
		if(enable) cgSetParameterSettingMode(g_vid_system.cg_context, CG_DEFERRED_PARAMETER_SETTING);
		else cgSetParameterSettingMode(g_vid_system.cg_context, CG_IMMEDIATE_PARAMETER_SETTING);
		CHECK_CG_ERR;
		RETURN_TRUE;
	}
	RETURN_FALSE;
}

Bool vid_sys_manage_texture_param(Bool enable)
{
	if(g_vid_system.cg_context)
	{
		cgGLSetManageTextureParameters(g_vid_system.cg_context, enable);
		CHECK_CG_ERR;
		RETURN_TRUE;
	}
	RETURN_FALSE;
}

Bool vid_sys_render(VID_PRIMITIVE_TYPE type, int vertices)
{
	if(vertices)
	{
		// ����������
		if(g_vid_system.indices_arr_id) glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_vid_system.indices_arr_id);
		// ��Ⱦ
		glDrawElements(type, vertices, g_vid_system.indices_type, (void*)g_vid_system.indices_offset);
		CHECK_GL_ERR;

		if(IS_DEBUG && g_vid_system.indices_arr_id) glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		RETURN_TRUE;
	}
	RETURN_FALSE;
}
