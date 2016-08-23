// 110106 检查

#ifndef __MS3D_h__
#define __MS3D_h__

#include "types.h"

#ifdef __cplusplus
extern "C"
{
#endif

// 最大名称长度
#define MS3D_MAX_NAME_SIZE				32
// 最大纹理路径长度
#define MS3D_MAX_TEXTURE_FILENAME_SIZE	128

// 结构大小
#define MS3D_VERTEX_SIZE		15
#define MS3D_TRIANGLE_SIZE		70
#define MS3D_MATERIAL_SIZE		361
#define MS3D_JOINT_SIZE			93
#define MS3D_FRAME_SIZE			16
#define MS3D_VERTEX_EXTRA_SIZE	6

// 顶点结构
typedef struct _ms3d_vertex
{
	float	vertex_pos[3];
	char	bone_id;
	// vertex extra
	char	bone_ids[3];
	uchar	weight[3];
} ms3d_vertex_t;

// 三角形结构
typedef struct _ms3d_triangle
{
	ushort	vertex_id[3];
	float	vertex_normal[9];
	float	s[3];
	float	t[3];
	uchar	smooth_group;
	uchar	group_id;
} ms3d_triangle_t;

// 组结构
typedef struct _ms3d_group
{
	char	name[MS3D_MAX_NAME_SIZE];
	ushort	group_triangles;
	char	material_id;

	ushort	*tri_id;
} ms3d_group_t;

// 材质结构
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

// 关键帧结构
typedef struct _ms3d_frame
{
	float	time;
	float	key[3];
} ms3d_frame_t;

// 骨骼(接合点)结构
typedef struct _ms3d_joint
{
	char	name[MS3D_MAX_NAME_SIZE];
	char	parent_name[MS3D_MAX_NAME_SIZE];
	float	rot[3];
	float	pos[3];
	ushort	rot_count;
	ushort	pos_count;

	ms3d_frame_t	*rotates;			// 旋转关键帧
	ms3d_frame_t	*translates;		// 平移关键帧
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