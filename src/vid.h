// 101021 使用短函数名
// 101022 添加 CG Buffer
// 101119 改为 Vid

#ifndef __Vid_h__
#define __Vid_h__

#include "types.h"
#include "refcount.h"
#include <gl/glew.h>
#include <gl/gl.h>
#include "Cg/cg.h"
#include "Cg/cgGL.h"

#ifdef __cplusplus
extern "C"
{
#endif

//////////////////////////////////////////////////////////////////////////
// 数值格式
typedef GLenum VID_VALUE_FORMAT;

//////////////////////////////////////////////////////////////////////////
// 图元类型
typedef GLenum VID_PRIMITIVE_TYPE;

// 系统
typedef struct _vid_system
{
	CGcontext			cg_context;		// CG 系统环境
	GLuint				indices_arr_id;	// 索引缓存 ID
	VID_VALUE_FORMAT	indices_type;	// 索引数据类型
	uint				indices_offset;	// 索引的偏移量
} vid_system_t;

extern vid_system_t g_vid_system;

// 创建系统
Bool vid_sys_create();
// 销毁系统
Bool vid_sys_destroy();
// 设置延迟参数传递
Bool vid_sys_deferred_param(Bool enable);
// 设置托管纹理参数
Bool vid_sys_manage_texture_param(Bool enable);
// 渲染
Bool vid_sys_render(VID_PRIMITIVE_TYPE type, int vertices);

//////////////////////////////////////////////////////////////////////////
// 像素顺序
typedef GLenum VID_PIXEL_ORDER;
// 颜色格式
typedef GLint VID_COLOR_FORMAT;

// 用途
typedef GLenum VID_USAGE;

// 纹理
typedef struct _vid_texture
{
	refcount_def;

	GLenum		target;
	GLuint		tex_id;

	uint		levels;
	uint		width;
	uint		height;
	uint		depth;

	Bool		compressed;

	VID_COLOR_FORMAT	iformat;
	VID_PIXEL_ORDER		format;
	VID_VALUE_FORMAT	type;
} vid_texture_t;

// 创建空白 2D 纹理
Bool vid_tex_create_2D(vid_texture_t **tex,
					   uint levels, GLint ifmt,
					   const void *pix, uint pixSize,
					   GLenum fmt, GLenum type, Bool compressed,
					   uint width, uint height, Bool autoMipmap);
// 从内存中 DDS 文件创建纹理
Bool vid_tex_create_dds(vid_texture_t **tex, const void *ptr, uint size, Bool autoMipmap);
// 从磁盘中 DDS 文件创建纹理
Bool vid_tex_create_dds_file(vid_texture_t **tex, const wchar_t *file, Bool autoMipmap);
// 释放纹理
Bool vid_tex_release(vid_texture_t **tex);
// 生成 Mipmap 纹理链
Bool vid_tex_gen_mipmap(vid_texture_t *tex);
// 含有 Mipmap 的线性采样
Bool vid_tex_linear_mipmap(vid_texture_t *tex);
// 去除边缘
Bool vid_tex_wrap_edge(vid_texture_t *tex);
// 各向异性过滤等级
Bool vid_tex_anisotropy(vid_texture_t *tex, int p);

//////////////////////////////////////////////////////////////////////////
// 渲染目标接入点
typedef GLenum VID_ATTACH_POINT;

// 渲染目标
typedef struct _vid_rendertar
{
	refcount_def;
	GLuint			rt_id;
	GLuint			default_depth;
	GLuint			default_stencil;
} vid_rendertar_t;

// 创建渲染目标对象
Bool vid_rt_create(vid_rendertar_t **rt);
// 销毁渲染目标对象
Bool vid_rt_release(vid_rendertar_t **rt);
// 设置为当前渲染目标
Bool vid_rt_enable(vid_rendertar_t *rt, const VID_ATTACH_POINT points[16], int pcount);
// 恢复默认渲染目标
void vid_rt_restore();
// 设置颜色接入纹理
Bool vid_rt_attach_texture(vid_rendertar_t *rt, VID_ATTACH_POINT point, vid_texture_t *tex, uint level);
// 设置默认深度接入纹理
Bool vid_rt_attach_depth_default(vid_rendertar_t *rt, VID_PIXEL_ORDER fmt, uint width, uint height);

//////////////////////////////////////////////////////////////////////////
// 缓存类型(顶点属性/索引)
typedef enum _VID_ARRAY_TYPE
{
	VID_VERTEX_ATTRIB,
	VID_INDEX,
} VID_ARRAY_TYPE;

// 顶点属性/索引缓存
typedef struct _vid_array
{
	refcount_def;
	GLenum			target;		// 内部对象类型
	GLuint			array_id;	// 内部 ID
	uint			stride;		// 跨度
} vid_array_t;

// 创建缓存数据
Bool vid_arr_create(vid_array_t **arr, VID_ARRAY_TYPE type, VID_USAGE usage, uint stride, const void *data, uint size);
// 销毁缓存数据
Bool vid_arr_release(vid_array_t **arr);
// 写入数据到缓存
Bool vid_arr_write(vid_array_t *arr, uint offset, const void *data, uint size);
// 设置为当前绘图顶点索引
Bool vid_arr_as_indices(vid_array_t *arr, VID_VALUE_FORMAT fmt, uint offset);

//////////////////////////////////////////////////////////////////////////
typedef CGbufferusage VID_BUFFER_USAGE;

// GPU 缓存参数
typedef struct _vid_buffer
{
	refcount_def;
	CGbuffer		buffer;
} vid_buffer_t;

Bool vid_buf_create(vid_buffer_t **buf, VID_BUFFER_USAGE usage, const void *data, uint size);
Bool vid_buf_release(vid_buffer_t **buf);
Bool vid_buf_write(vid_buffer_t *buf, uint offset, const void *data, uint size);

//////////////////////////////////////////////////////////////////////////
// GPU 处理程序类型
typedef enum _VID_PROGRAM_TYPE
{
	VID_VERTEX_PROGRAM,
	VID_FRAGMENT_PROGRAM,
	VID_GEOMETRY_PROGRAM,
} VID_PROGRAM_TYPE;

// GPU 处理程序
typedef struct _vid_program
{
	refcount_def;
	CGprogram		program;
} vid_program_t;

// 创建 GPU 处理程序
Bool vid_prog_create(vid_program_t **pro, VID_PROGRAM_TYPE type, Bool file, Bool src, const char *code, const char *entry);
// 销毁 GPU 处理程序
Bool vid_prog_release(vid_program_t **pro);
// 设置为当前 GPU 处理程序
Bool vid_prog_enable(vid_program_t *pro);
// 更新延迟参数
Bool vid_prog_update_params(vid_program_t *pro);

//////////////////////////////////////////////////////////////////////////
// GPU 处理程序参数
typedef struct _vid_param
{
	refcount_def;
	vid_program_t	*program;
	CGparameter		param;
} vid_param_t;

// 根据程序获得对应标识符的参数
Bool vid_param_bind(vid_param_t **param, vid_program_t *pro, const char *name);
Bool vid_param_release(vid_param_t **param);
// 启用顶点属性参数
Bool vid_param_enable_vertices(vid_param_t *param, Bool enable);
// 启用纹理参数
Bool vid_param_enable_texture(vid_param_t *param, Bool enable);
// 设置参数的值
Bool vid_param_set_float(vid_param_t *param, float val);
Bool vid_param_set_float_2(vid_param_t *param, float val1, float val2);
Bool vid_param_set_float_3(vid_param_t *param, float val1, float val2, float val3);
Bool vid_param_set_float_4(vid_param_t *param, float val1, float val2, float val3, float val4);
Bool vid_param_set_matrix(vid_param_t *param, const float *val);
Bool vid_param_set_float_array(vid_param_t *param, const float *val, int elements);
Bool vid_param_set_buffer(vid_param_t *param, vid_buffer_t *buf);
// 设置顶点属性缓存
Bool vid_param_set_vertices(vid_param_t *param, vid_array_t *arr, VID_VALUE_FORMAT fmt, int fsize, uint offset);
// 设置纹理采样器参数
Bool vid_param_set_texture(vid_param_t *param, vid_texture_t *tex);

#ifdef __cplusplus
}
#endif

#endif