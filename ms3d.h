// 110106 ���

#ifndef __MS3D_h__
#define __MS3D_h__

#include "types.h"

#ifdef __cplusplus
extern "C"
{
#endif

// ������Ƴ���
#define MS3D_MAX_NAME_SIZE				32
// �������·������
#define MS3D_MAX_TEXTURE_FILENAME_SIZE	128

// �ṹ��С
#define MS3D_VERTEX_SIZE		15
#define MS3D_TRIANGLE_SIZE		70
#define MS3D_MATERIAL_SIZE		361
#define MS3D_JOINT_SIZE			93
#define MS3D_FRAME_SIZE			16
#define MS3D_VERTEX_EXTRA_SIZE	6

// ����ṹ
typedef struct _ms3d_vertex
{
	float	vertex_pos[3];
	char	bone_id;
	// vertex extra
	char	bone_ids[3];
	uchar	weight[3];
} ms3d_vertex_t;

// �����νṹ
typedef struct _ms3d_triangle
{
	ushort	vertex_id[3];
	float	vertex_normal[9];
	float	s[3];
	float	t[3];
	uchar	smooth_group;
	uchar	group_id;
} ms3d_triangle_t;

// ��ṹ
typedef struct _ms3d_group
{
	char	name[MS3D_MAX_NAME_SIZE];
	ushort	group_triangles;
	char	material_id;

	ushort	*tri_id;
} ms3d_group_t;

// ���ʽṹ
typedef struct _ms3d_material
{
	char	name[MS3D_MAX_NAME_SIZE];
	float	ambient[4];
	float	diffuse[4];
	float	specular[4];
	float	emissive[4];
	float	shininess;
	float	transparency;
	uchar	mode;
	char	diffuse_texture[MS3D_MAX_TEXTURE_FILENAME_SIZE];
	char	alpha_texture[MS3D_MAX_TEXTURE_FILENAME_SIZE];
} ms3d_material_t;

// �ؼ�֡�ṹ
typedef struct _ms3d_frame
{
	float	time;
	float	key[3];
} ms3d_frame_t;

// ����(�Ӻϵ�)�ṹ
typedef struct _ms3d_joint
{
	char	name[MS3D_MAX_NAME_SIZE];
	char	parent_name[MS3D_MAX_NAME_SIZE];
	float	rot[3];
	float	pos[3];
	ushort	rot_count;
	ushort	pos_count;

	ms3d_frame_t	*rotates;			// ��ת�ؼ�֡
	ms3d_frame_t	*translates;		// ƽ�ƹؼ�֡
} ms3d_joint_t;

typedef struct _ms3d_model
{
	ms3d_vertex_t	*vertices;
	ms3d_triangle_t	*tris;
	ms3d_group_t	*groups;
	ushort			*tri_ids;
	ms3d_material_t	*mtrls;
	ms3d_joint_t	*joints;
	ms3d_frame_t	*rotates;
	ms3d_frame_t	*translates;

	ushort numVerts, numTris, numGroups, numMtrls, numJoints;
	uint numRotates, numTranslates;

	float	fps;
	int		totalFrms;
} ms3d_model_t;

Bool ms3d_read_buf(void *buf, uint buflen,
				   ms3d_vertex_t vertices[],
				   ms3d_triangle_t triangles[],
				   ms3d_group_t groups[], ushort triangle_id[],
				   ms3d_material_t materials[],
				   ms3d_joint_t joints[], ms3d_frame_t rotates[], ms3d_frame_t translates[],
				   ushort *pVerticesCnt, ushort *pTrianglesCnt, ushort *pGroupsCnt, ushort *pMtrlCnt,
				   ushort *pJointsCnt, uint *pRotatesCnt, uint *pTranslatesCnt,
				   float *fps, int *total_frms);
Bool ms3d_load(void *buf, uint buflen, ms3d_model_t *model);
Bool ms3d_unload(ms3d_model_t *model);

#ifdef __cplusplus
}
#endif

#endif