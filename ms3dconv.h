// 110106
// 110107
// 110112 为动画链函数添加骨骼影响参数, 重写vertex_to_infl
// 110128 拷贝到当前工程并修改

#ifndef __MS3DConvert_h__
#define __MS3DConvert_h__

#include "ms3d.h"
#include "mdlcalc.h"

#ifdef __cplusplus
extern "C"
{
#endif

Bool ms3d_to_mdl_mesh_pre(const ms3d_model_t *ms3d,
						  mdl_mesh_t *mesh);

Bool ms3d_to_mdl_mesh(const ms3d_model_t *ms3d,
					  mdl_mesh_t *mesh);


Bool ms3d_to_mdl_skel_pre(const ms3d_model_t *ms3d, mdl_skeleton_t *skel);

Bool ms3d_to_mdl_skel(const ms3d_model_t *ms3d, mdl_skeleton_t *skel);

Bool ms3d_to_mdl_track_pre(const ms3d_model_t *ms3d, mdl_track_t *track,
						   const ushort *bone_aff_ids, ushort bone_aff_count);

Bool ms3d_to_mdl_track(const ms3d_model_t *ms3d, mdl_track_t *track);

#ifdef __cplusplus
}
#endif

#endif