// 101214: 修改并注释
// 101216: 修改
// 101221: 重构
// 101222: 重构
// 101226: 新增,改为mdlcalc
// 101227: 再次重构
// 101229
// 110103
// 110106
// 110107
// 110120: 改进顶点融合, 跳过静止顶点

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
// 权重串结束标志
#define MDL_INFL_BREAK	((ushort)65535)

// 平滑组
typedef struct _mdl_group
{
	uint	indi_begin;				// 索引数据中起始下标
	uint	indi_len;				// 索引个数
	ushort	material_id;			// 材质索引
} mdl_group_t;

// 顶点骨骼权重
typedef struct _mdl_influence
{
	ushort	bone_id[4];
	float	weight[3];
} mdl_influence_t;

// 网格
typedef struct _mdl_mesh
{
	refcount_def;
	// 流数据
	vec3			*vertices;		// 顶点坐标
	vec2			*texcoords;		// 顶点纹理坐标
	vec3			*normals;		// 顶点法线
	vec3			*tangents;		// 顶点切线
	vec3			*binormals;		// 顶点副切线
	mdl_influence_t *influences;	// 顶点权重
	ushort			vert_count;		// 顶点个数

	// 索引数据
	ushort			*indices;		// 用于映射顶点的索引
	uint			indi_count;

	// 分组数据
	mdl_group_t		*groups;
	ushort			group_count;	// 组个数
} mdl_mesh_t;

// 网格融合器
// 不能单独使用, 仅存在于模型节点中
typedef struct _mdl_mesh_blender
{
	mdl_mesh_t			*mesh;				// 绑定的网格

	// 供渲染用的顶点和法线流
	vec3				*vertex_stream;		// 顶点融合后的顶点列表
	vec3				*normal_stream;		// 顶点融合后的法线列表
	vec3				*tangent_stream;	// 顶点融合后的切线列表
	vec3				*binormal_stream;	// 顶点融合后的副切线列表
} mdl_mesh_blender_t;

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// 键
typedef struct _mdl_key
{
	quat	rot;					// 朝向/旋转(四元数)
	vec3	pos;					// 位置/位移
} mdl_key_t;


// 单个骨骼
typedef struct _mdl_bone
{
	mdl_key_t	key;
	int			parent_id;			// 父骨骼下标
} mdl_bone_t;

// 骨架(包含一个对象的所有骨骼)
typedef struct _mdl_skeleton
{
	refcount_def;
	mdl_bone_t	*bones;				// 骨骼列表(按父子关系前后排列)
	ushort		bone_count;			// 含有的骨骼个数
} mdl_skeleton_t;


// 关键帧
typedef struct _mdl_keyframe
{
	mdl_key_t	key;
	float		time;				// 时间戳
} mdl_keyframe_t;

// 关键帧列表
typedef struct _mdl_frames
{
	mdl_keyframe_t	*frames;		// 帧列表, 按时间前后排列
	uint			frm_count;		// 帧总个数
} mdl_frames_t;

// 动画链
typedef struct _mdl_track
{
	refcount_def;
	mdl_frames_t	*tracks;		// 影响的骨骼的关键帧列表
	ushort			*bone_aff_ids;	// 影响的骨骼下标列表
	ushort			bone_aff_count;	// 影响的骨骼个数

	mdl_keyframe_t	*keyfrm_pool;
	uint			keyfrm_count;
} mdl_track_t;


// 跳转结构
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

// 动画
typedef struct _mdl_animate
{
	refcount_def;

	mdl_track_t	*track;				// 绑定的动画链

	Bool		b_goto;
	mdl_goto_t	goto_proc;

	float		time;				// 当前时间

	uint		*target_frms;		// 用于加速寻帧的帧下标列表
	mdl_key_t	*keys_interp;		// 保存当前帧插值结果
} mdl_animate_t;

//////////////////////////////////////////////////////////////////////////
// 模型节点
typedef struct _mdl_node
{
	// 定义引用计数
	refcount_def;

	// 第一个子节点
	struct _mdl_node	*first_child;
	// 当前节点的骨架, 和第一个子节点的链接骨骼
	ushort				first_child_bone;

	// 下一个兄弟节点
	struct _mdl_node	*next_sibling;
	// 父节点的骨架, 和下一个子节点的链接骨骼
	ushort				next_sibling_bone;

	// 当前节点绑定的骨架
	mdl_skeleton_t		*skeleton;

	// 骨骼动画控制器
	mdl_animate_t		**anim_ctrl;		// 注意这个是指针列表
	ushort				anim_ctrl_count;

	// 网格融合器列表
	mdl_mesh_blender_t	*mesh_blender;		// 这个是实体列表, 由模型节点完全控制
	ushort				mesh_blender_count;

	// 用于存储计算用的矩阵
	//! 更换骨架后需要 realloc, 并 skel_update 骨架
	mat4				*mat_iglobal_skel;	// 用于计算顶点融合的全局骨架矩阵
	mat4				*mat_global_anim;	// 经过动画的全局骨架矩阵
	mat4				*mat_local_skel;	// 用于计算动画的自身骨架矩阵
	mat4				*mat_local_anim;	// 临时的用于储存骨架动画计算的自身动画矩阵
	mat4				*mat_anim_list;		// 存储动画位移的矩阵列表
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
// 把键转换成矩阵
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
// 开启 mdl 结构工厂
Bool mdl_factory_open();
// 关闭 mdl 结构工厂
Bool mdl_factory_close();

#ifdef __cplusplus
}
#endif

#endif
