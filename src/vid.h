// 101021 ʹ�ö̺�����
// 101022 ��� CG Buffer
// 101119 ��Ϊ Vid

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
// ��ֵ��ʽ
typedef GLenum VID_VALUE_FORMAT;

//////////////////////////////////////////////////////////////////////////
// ͼԪ����
typedef GLenum VID_PRIMITIVE_TYPE;

// ϵͳ
typedef struct _vid_system
{
	CGcontext			cg_context;		// CG ϵͳ����
	GLuint				indices_arr_id;	// �������� ID
	VID_VALUE_FORMAT	indices_type;	// ������������
	uint				indices_offset;	// ������ƫ����
} vid_system_t;

extern vid_system_t g_vid_system;

// ����ϵͳ
Bool vid_sys_create();
// ����ϵͳ
Bool vid_sys_destroy();
// �����ӳٲ�������
Bool vid_sys_deferred_param(Bool enable);
// �����й��������
Bool vid_sys_manage_texture_param(Bool enable);
// ��Ⱦ
Bool vid_sys_render(VID_PRIMITIVE_TYPE type, int vertices);

//////////////////////////////////////////////////////////////////////////
// ����˳��
typedef GLenum VID_PIXEL_ORDER;
// ��ɫ��ʽ
typedef GLint VID_COLOR_FORMAT;

// ��;
typedef GLenum VID_USAGE;

// ����
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

// �����հ� 2D ����
Bool vid_tex_create_2D(vid_texture_t **tex,
					   uint levels, GLint ifmt,
					   const void *pix, uint pixSize,
					   GLenum fmt, GLenum type, Bool compressed,
					   uint width, uint height, Bool autoMipmap);
// ���ڴ��� DDS �ļ���������
Bool vid_tex_create_dds(vid_texture_t **tex, const void *ptr, uint size, Bool autoMipmap);
// �Ӵ����� DDS �ļ���������
Bool vid_tex_create_dds_file(vid_texture_t **tex, const wchar_t *file, Bool autoMipmap);
// �ͷ�����
Bool vid_tex_release(vid_texture_t **tex);
// ���� Mipmap ������
Bool vid_tex_gen_mipmap(vid_texture_t *tex);
// ���� Mipmap �����Բ���
Bool vid_tex_linear_mipmap(vid_texture_t *tex);
// ȥ����Ե
Bool vid_tex_wrap_edge(vid_texture_t *tex);
// �������Թ��˵ȼ�
Bool vid_tex_anisotropy(vid_texture_t *tex, int p);

//////////////////////////////////////////////////////////////////////////
// ��ȾĿ������
typedef GLenum VID_ATTACH_POINT;

// ��ȾĿ��
typedef struct _vid_rendertar
{
	refcount_def;
	GLuint			rt_id;
	GLuint			default_depth;
	GLuint			default_stencil;
} vid_rendertar_t;

// ������ȾĿ�����
Bool vid_rt_create(vid_rendertar_t **rt);
// ������ȾĿ�����
Bool vid_rt_release(vid_rendertar_t **rt);
// ����Ϊ��ǰ��ȾĿ��
Bool vid_rt_enable(vid_rendertar_t *rt, const VID_ATTACH_POINT points[16], int pcount);
// �ָ�Ĭ����ȾĿ��
void vid_rt_restore();
// ������ɫ��������
Bool vid_rt_attach_texture(vid_rendertar_t *rt, VID_ATTACH_POINT point, vid_texture_t *tex, uint level);
// ����Ĭ����Ƚ�������
Bool vid_rt_attach_depth_default(vid_rendertar_t *rt, VID_PIXEL_ORDER fmt, uint width, uint height);

//////////////////////////////////////////////////////////////////////////
// ��������(��������/����)
typedef enum _VID_ARRAY_TYPE
{
	VID_VERTEX_ATTRIB,
	VID_INDEX,
} VID_ARRAY_TYPE;

// ��������/��������
typedef struct _vid_array
{
	refcount_def;
	GLenum			target;		// �ڲ���������
	GLuint			array_id;	// �ڲ� ID
	uint			stride;		// ���
} vid_array_t;

// ������������
Bool vid_arr_create(vid_array_t **arr, VID_ARRAY_TYPE type, VID_USAGE usage, uint stride, const void *data, uint size);
// ���ٻ�������
Bool vid_arr_release(vid_array_t **arr);
// д�����ݵ�����
Bool vid_arr_write(vid_array_t *arr, uint offset, const void *data, uint size);
// ����Ϊ��ǰ��ͼ��������
Bool vid_arr_as_indices(vid_array_t *arr, VID_VALUE_FORMAT fmt, uint offset);

//////////////////////////////////////////////////////////////////////////
typedef CGbufferusage VID_BUFFER_USAGE;

// GPU �������
typedef struct _vid_buffer
{
	refcount_def;
	CGbuffer		buffer;
} vid_buffer_t;

Bool vid_buf_create(vid_buffer_t **buf, VID_BUFFER_USAGE usage, const void *data, uint size);
Bool vid_buf_release(vid_buffer_t **buf);
Bool vid_buf_write(vid_buffer_t *buf, uint offset, const void *data, uint size);

//////////////////////////////////////////////////////////////////////////
// GPU �����������
typedef enum _VID_PROGRAM_TYPE
{
	VID_VERTEX_PROGRAM,
	VID_FRAGMENT_PROGRAM,
	VID_GEOMETRY_PROGRAM,
} VID_PROGRAM_TYPE;

// GPU �������
typedef struct _vid_program
{
	refcount_def;
	CGprogram		program;
} vid_program_t;

// ���� GPU �������
Bool vid_prog_create(vid_program_t **pro, VID_PROGRAM_TYPE type, Bool file, Bool src, const char *code, const char *entry);
// ���� GPU �������
Bool vid_prog_release(vid_program_t **pro);
// ����Ϊ��ǰ GPU �������
Bool vid_prog_enable(vid_program_t *pro);
// �����ӳٲ���
Bool vid_prog_update_params(vid_program_t *pro);

//////////////////////////////////////////////////////////////////////////
// GPU ����������
typedef struct _vid_param
{
	refcount_def;
	vid_program_t	*program;
	CGparameter		param;
} vid_param_t;

// ���ݳ����ö�Ӧ��ʶ���Ĳ���
Bool vid_param_bind(vid_param_t **param, vid_program_t *pro, const char *name);
Bool vid_param_release(vid_param_t **param);
// ���ö������Բ���
Bool vid_param_enable_vertices(vid_param_t *param, Bool enable);
// �����������
Bool vid_param_enable_texture(vid_param_t *param, Bool enable);
// ���ò�����ֵ
Bool vid_param_set_float(vid_param_t *param, float val);
Bool vid_param_set_float_2(vid_param_t *param, float val1, float val2);
Bool vid_param_set_float_3(vid_param_t *param, float val1, float val2, float val3);
Bool vid_param_set_float_4(vid_param_t *param, float val1, float val2, float val3, float val4);
Bool vid_param_set_matrix(vid_param_t *param, const float *val);
Bool vid_param_set_float_array(vid_param_t *param, const float *val, int elements);
Bool vid_param_set_buffer(vid_param_t *param, vid_buffer_t *buf);
// ���ö������Ի���
Bool vid_param_set_vertices(vid_param_t *param, vid_array_t *arr, VID_VALUE_FORMAT fmt, int fsize, uint offset);
// �����������������
Bool vid_param_set_texture(vid_param_t *param, vid_texture_t *tex);

#ifdef __cplusplus
}
#endif

#endif