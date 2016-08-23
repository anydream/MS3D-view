// 101003 检查
// 101124 检查并修改

#include "vid.h"
#include "viddebug.h"
#include "factory.h"
#include <string.h>

extern factory_t g_factory_vid_rendertar;

#define vid_fac_rt_yield			((vid_rendertar_t*)factory_yield(&g_factory_vid_rendertar))
#define vid_fac_rt_recycle(_p)		(factory_recycle(&g_factory_vid_rendertar, _p))

Bool vid_rt_create(vid_rendertar_t **rt)
{
	if(rt)
	{
		vid_rendertar_t temp;

		// 此时可以填写结构中的数据
		{
			GLuint id;

			glGenFramebuffers(1, &id);
			if(!id) RETURN_FALSE;

			temp.rt_id = id;
			temp.default_depth = 0;
			temp.default_stencil = 0;
		}

		// 创建结构, 可以从工厂中生产一个结构
		*rt = vid_fac_rt_yield;

		memmove(*rt, &temp, sizeof(vid_rendertar_t));

		// 初始化引用计数为0
		refcount_init(*rt);

		// 添加一次引用
		reference(*rt);

		RETURN_TRUE;
	}
	RETURN_FALSE;
}

Bool vid_rt_release(vid_rendertar_t **rt)
{
	if(rt)
	{
		// 如果引用为0
		if(reference_release(*rt))
		{
			// 释放成员数据
			if((*rt)->default_depth) glDeleteRenderbuffers(1, &(*rt)->default_depth);
			if((*rt)->default_stencil) glDeleteRenderbuffers(1, &(*rt)->default_stencil);
			glDeleteFramebuffers(1, &(*rt)->rt_id);
			CHECK_GL_ERR;

			// 释放结构实体, 可以让工厂回收
			vid_fac_rt_recycle(*rt);
		}
		*rt = 0;
		RETURN_TRUE;
	}
	RETURN_FALSE;
}

Bool vid_rt_enable(vid_rendertar_t *rt, const VID_ATTACH_POINT points[16], int pcount)
{
	if(rt && rt->rt_id && pcount <= 16)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, rt->rt_id);

		if(points && pcount) glDrawBuffers(pcount, points);
		CHECK_GL_ERR;

		RETURN_TRUE;
	}
	RETURN_FALSE;
}

void vid_rt_restore()
{
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Bool vid_rt_attach_texture(vid_rendertar_t *rt, VID_ATTACH_POINT point, vid_texture_t *tex, uint level)
{
	if(rt && rt->rt_id && tex && tex->target == GL_TEXTURE_2D)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, rt->rt_id);
		glFramebufferTexture2D(GL_FRAMEBUFFER, point, GL_TEXTURE_2D, tex->tex_id, level);

		CHECK_GL_ERR;

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		RETURN_TRUE;
	}
	RETURN_FALSE;
}

Bool vid_rt_attach_depth_default(vid_rendertar_t *rt, VID_PIXEL_ORDER fmt, uint width, uint height)
{
	if(rt && rt->rt_id && !rt->default_depth && width && height)
	{
		GLuint depth_id;

		glGenRenderbuffers(1, &depth_id);
		if(!depth_id) RETURN_FALSE;

		rt->default_depth = depth_id;

		glBindRenderbuffer(GL_RENDERBUFFER, depth_id);
		glRenderbufferStorage(GL_RENDERBUFFER, fmt, width, height);

		CHECK_GL_ERR;

		glBindFramebuffer(GL_FRAMEBUFFER, rt->rt_id);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_id);

		CHECK_GL_ERR;

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		RETURN_TRUE;
	}
	RETURN_FALSE;
}