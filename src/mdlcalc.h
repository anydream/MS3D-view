// 101214: �޸Ĳ�ע��
// 101216: �޸�
// 101221: �ع�
// 101222: �ع�
// 101226: ����,��Ϊmdlcalc
// 101227: �ٴ��ع�
// 101229
// 110103
// 110106
// 110107
// 110120: �Ľ������ں�, ������ֹ����

#ifndef __ModelCalc_h__
#define __ModelCalc_h__

#include "types.h"
#include "math3D.h"
#include "refcount.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef ushort ushort4[4];
//////////////////////////////////////////////////////////////////////////
// Ȩ�ش�������־
#define MDL_INFL_BREAK	((ushort)65535)

// ƽ����
typedef struct _mdl_group
{
	uint	indi_begin;				// ������������ʼ�±�
	uint	indi_len;				// ��������
	ushort	material_id;			// ��������
} mdl_group_t;

// �������Ȩ��
typedef struct _mdl_influence
{
	ushort	bone_id[4];
	float	weight[3];
} mdl_influence_t;

// ����
typedef struct _mdl_mesh
{
	refcount_def;
	// ������
	vec3			*vertices;		// ��������
	vec2			*texcoords;		// ������������
	vec3			*normals;		// ���㷨��
	vec3			*tangents;		// ��������
	vec3			*binormals;		// ���㸱����
	mdl_influence_t *influences;	// ����Ȩ��
	ushort			vert_count;		// �������

	// ��������
	ushort			*indices;		// ����ӳ�䶥�������
	uint			indi_count;

	// ��������
	mdl_group_t		*groups;
	ushort			group_count;	// �����
} mdl_mesh_t;

// �����ں���
// ���ܵ���ʹ��, ��������ģ�ͽڵ���
typedef struct _mdl_mesh_blender
{
	mdl_mesh_t			*mesh;				// �󶨵�����

	// ����Ⱦ�õĶ���ͷ�����
	vec3				*vertex_stream;		// �����ںϺ�Ķ����б�
	vec3				*normal_stream;		// �����ںϺ�ķ����б�
	vec3				*tangent_stream;	// �����ںϺ�������б�
	vec3				*binormal_stream;	// �����ںϺ�ĸ������б�
} mdl_mesh_blender_t;

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// ��
typedef struct _mdl_key
{
	quat	rot;					// ����/��ת(��Ԫ��)
	vec3	pos;					// λ��/λ��
} mdl_key_t;


// ��������
typedef struct _mdl_bone
{
	mdl_key_t	key;
	int			parent_id;			// �������±�
} mdl_bone_t;

// �Ǽ�(����һ����������й���)
typedef struct _mdl_skeleton
{
	refcount_def;
	mdl_bone_t	*bones;				// �����б�(�����ӹ�ϵǰ������)
	ushort		bone_count;			// ���еĹ�������
} mdl_skeleton_t;


// �ؼ�֡
typedef struct _mdl_keyframe
{
	mdl_key_t	key;
	float		time;				// ʱ���
} mdl_keyframe_t;

// �ؼ�֡�б�
typedef struct _mdl_frames
{
	mdl_keyframe_t	*frames;		// ֡�б�, ��ʱ��ǰ������
	uint			frm_count;		// ֡�ܸ���
} mdl_frames_t;

// ������
typedef struct _mdl_track
{
	refcount_def;
	mdl_frames_t	*tracks;		// Ӱ��Ĺ����Ĺؼ�֡�б�
	ushort			*bone_aff_ids;	// Ӱ��Ĺ����±��б�
	ushort			bone_aff_count;	// Ӱ��Ĺ�������

	mdl_keyframe_t	*keyfrm_pool;
	uint			keyfrm_count;
} mdl_track_t;


// ��ת�ṹ
typedef struct _mdl_goto
{
	mdl_key_t	*start_keys;
	float		start_time;

	mdl_key_t	*end_keys;
	float		end_time;

	float		time_total;
	float		time_curr;

	float		weight;
} mdl_goto_t;

// ����
typedef struct _mdl_animate
{
	refcount_def;

	mdl_track_t	*track;				// �󶨵Ķ�����

	Bool		b_goto;
	mdl_goto_t	goto_proc;

	float		time;				// ��ǰʱ��

	uint		*target_frms;		// ���ڼ���Ѱ֡��֡�±��б�
	mdl_key_t	*keys_interp;		// ���浱ǰ֡��ֵ���
} mdl_animate_t;

//////////////////////////////////////////////////////////////////////////
// ģ�ͽڵ�
typedef struct _mdl_node
{
	// �������ü���
	refcount_def;

	// ��һ���ӽڵ�
	struct _mdl_node	*first_child;
	// ��ǰ�ڵ�ĹǼ�, �͵�һ���ӽڵ�����ӹ���
	ushort				first_child_bone;

	// ��һ���ֵܽڵ�
	struct _mdl_node	*next_sibling;
	// ���ڵ�ĹǼ�, ����һ���ӽڵ�����ӹ���
	ushort				next_sibling_bone;

	// ��ǰ�ڵ�󶨵ĹǼ�
	mdl_skeleton_t		*skeleton;

	// ��������������
	mdl_animate_t		**anim_ctrl;		// ע�������ָ���б�
	ushort				anim_ctrl_count;

	// �����ں����б�
	mdl_mesh_blender_t	*mesh_blender;		// �����ʵ���б�, ��ģ�ͽڵ���ȫ����
	ushort				mesh_blender_count;

	// ���ڴ洢�����õľ���
	//! �����Ǽܺ���Ҫ realloc, �� skel_update �Ǽ�
	mat4				*mat_iglobal_skel;	// ���ڼ��㶥���ںϵ�ȫ�ֹǼܾ���
	mat4				*mat_global_anim;	// ����������ȫ�ֹǼܾ���
	mat4				*mat_local_skel;	// ���ڼ��㶯��������Ǽܾ���
	mat4				*mat_local_anim;	// ��ʱ�����ڴ���Ǽܶ������������������
	mat4				*mat_anim_list;		// �洢����λ�Ƶľ����б�
} mdl_node_t;

//////////////////////////////////////////////////////////////////////////
void mdl_vertex_blend(const vec3 vert, const mdl_influence_t *infl,
					  const mat4 *mat_global_anim_list,
					  vec3 vert_out);

void mdl_normal_blend(const vec3 norm, const mdl_influence_t *infl,
					  const mat4 *mat_global_anim_list,
					  vec3 norm_out);


Bool mdl_mesh_create(mdl_mesh_t **mesh);

void mdl_mesh_release(mdl_mesh_t **mesh);

Bool mdl_mesh_blend(const mdl_mesh_t *mesh, const mat4 *mat_global_anim_list,
					vec3 *vertices_out, vec3 *normals_out, vec3 *tangents_out, vec3 *binormals_out);

Bool mdl_mesh_infl_stream(const mdl_mesh_t *mesh, ushort4 **bone_ids, vec4 **bone_weights);

void mdl_mesh_infl_stream_free(ushort4 *bone_ids, vec4 *bone_weights);

Bool mdl_mesh_calc_tangent(mdl_mesh_t *mesh, Bool b_normal, Bool b_tangent, Bool b_binormal);

//////////////////////////////////////////////////////////////////////////
// �Ѽ�ת���ɾ���
#define mdl_key_matrix(_key, _mat)	matQuatTrans((_key).rot, (_key).pos, _mat)

Bool mdl_key_interp(const mdl_key_t *k1, const mdl_key_t *k2, float weight, mdl_key_t *key_out);


Bool mdl_skel_create(mdl_skeleton_t **skel);

void mdl_skel_release(mdl_skeleton_t **skel);

Bool mdl_skel_update(const mdl_skeleton_t *skel,
					 mat4 *mat_local_skel_list, mat4 *mat_iglobal_skel_list);

Bool mdl_skel_animate(const mdl_skeleton_t *skel,
					  const mat4 *mat_local_skel_list, const mat4 *mat_iglobal_skel_list,
					  const mat4 *mat_animate_list,
					  mat4 *mat_local_anim_list, mat4 *mat_global_anim_list);

Bool mdl_skel_final_matrix_array(const mdl_skeleton_t *skel, const mat4 *mat_global_anim_list,
								 float *mat_array, ushort mat_element_count);

Bool mdl_frames_get_id(const mdl_frames_t *frms, float time, uint target_frm,
					   uint *out_curr_id, uint *out_next_id, float *out_weight);

Bool mdl_frames_interp(const mdl_frames_t *frms, float time, uint *target_frm, mdl_key_t *key_out);


Bool mdl_track_create(mdl_track_t **track);

void mdl_track_release(mdl_track_t **track);

Bool mdl_track_interp(const mdl_track_t *track, float time, uint *target_frms, mdl_key_t *keys_out);


Bool mdl_anim_create(mdl_animate_t **anim);

void mdl_anim_release(mdl_animate_t **anim);

Bool mdl_anim_bind_track(mdl_animate_t *anim, mdl_track_t *track);

Bool mdl_anim_interp(mdl_animate_t *anim);

Bool mdl_anim_matrix(const mdl_animate_t *anim, mat4 *mat_animate_list);

Bool mdl_anim_goto(mdl_animate_t *anim, float target_time, float goto_time);

Bool mdl_anim_advance(mdl_animate_t *anim, float delta);

Bool mdl_anim_set_time(mdl_animate_t *anim, float target_time);

//////////////////////////////////////////////////////////////////////////
Bool mdl_node_create(mdl_node_t **node);

void mdl_node_release(mdl_node_t **node);

Bool mdl_node_bind_skeleton(mdl_node_t *node, mdl_skeleton_t *skel);

Bool mdl_node_release_skeleton(mdl_node_t *node);

Bool mdl_node_bind_animate(mdl_node_t *node, mdl_animate_t *anim);

Bool mdl_node_bind_mesh(mdl_node_t *node, mdl_mesh_t *mesh);

void mdl_node_advance(mdl_node_t *node, float delta);

void mdl_node_interp(mdl_node_t *node);

void mdl_node_blend(mdl_node_t *node);

//////////////////////////////////////////////////////////////////////////
// ���� mdl �ṹ����
Bool mdl_factory_open();
// �ر� mdl �ṹ����
Bool mdl_factory_close();

#ifdef __cplusplus
}
#endif

#endif
