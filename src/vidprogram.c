// 101002 检查
// 101003 检查
// 101124 检查

#include "vid.h"
#include "viddebug.h"
#include "ut.h"
#include "factory.h"
#include <string.h>

extern factory_t g_factory_vid_program;

#define vid_fac_prog_yield			((vid_program_t*)factory_yield(&g_factory_vid_program))
#define vid_fac_prog_recycle(_p)	(factory_recycle(&g_factory_vid_program, _p))

Bool vid_prog_create(vid_program_t **pro, VID_PROGRAM_TYPE type, Bool file, Bool src, const char *code, const char *entry)
{
	if(pro && g_vid_system.cg_context && code && entry)
	{
		vid_program_t temp;

		// 此时可以填写结构中的数据
		{
			CGprofile profile;
			CGprogram program;
			CGGLenum protype = (type == VID_VERTEX_PROGRAM ? CG_GL_VERTEX : (type == VID_FRAGMENT_PROGRAM ? CG_GL_FRAGMENT: CG_GL_GEOMETRY));

			// 获得最新 Profile 并启用
			profile = cgGLGetLatestProfile(protype);
			// 设置优化参数
			cgGLSetOptimalOptions(profile);
			cgGLEnableProfile(profile);

			// 创建处理程序
			if(file)
			{
				if(src) program = cgCreateProgramFromFile(g_vid_system.cg_context, CG_SOURCE, code, profile, entry, 0);
				else
				{
					wchar_t path[260];
					void *buf;
					uint len;

					_swprintf(path, L"%S", code);
					buf = ut_file_read(path, &len);
					if(buf)
					{
						char *str = (char*)ut_alloc(len + 1);
						memmove(str, buf, len);
						str[len] = 0;

						program = cgCreateProgram(g_vid_system.cg_context, CG_OBJECT, str, profile, entry, 0);

						ut_free(str);
						ut_file_free(buf);
					}
					else RETURN_FALSE;
				}
			}
			else program = cgCreateProgram(g_vid_system.cg_context, src ? CG_SOURCE : CG_OBJECT, code, profile, entry, 0);
			CHECK_CG_ERR;
			if(!program) RETURN_FALSE;

			// 如果没有编译, 则编译程序
			if(!cgIsProgramCompiled(program)) cgCompileProgram(program);

			// 加载程序
			cgGLLoadProgram(program);

			temp.program = program;
		}

		// 创建结构, 可以从工厂中生产一个结构
		*pro = vid_fac_prog_yield;

		memmove(*pro, &temp, sizeof(vid_program_t));

		// 初始化引用计数为0
		refcount_init(*pro);

		// 添加一次引用
		reference(*pro);

		RETURN_TRUE;
	}
	RETURN_FALSE;
}

Bool vid_prog_release(vid_program_t **pro)
{
	if(pro)
	{
		// 如果引用为0
		if(reference_release(*pro))
		{
			// 释放成员数据
			cgDestroyProgram((*pro)->program);
			CHECK_CG_ERR;

			// 释放结构实体, 可以让工厂回收
			vid_fac_prog_recycle(*pro);
		}
		*pro = 0;
		RETURN_TRUE;
	}
	RETURN_FALSE;
}

Bool vid_prog_enable(vid_program_t *pro)
{
	if(pro && pro->program)
	{
		// 绑定程序
		cgGLBindProgram(pro->program);
		RETURN_TRUE;
	}
	RETURN_FALSE;
}

Bool vid_prog_update_params(vid_program_t *pro)
{
	if(pro && pro->program)
	{
		// 更新参数
		cgUpdateProgramParameters(pro->program);
		CHECK_CG_ERR;
		RETURN_TRUE;
	}
	RETURN_FALSE;
}