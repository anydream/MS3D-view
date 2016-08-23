// 110128 镜像骨骼动画

#include "ut.h"
#include "ms3dconv.h"
#include <stdio.h>
#include <string.h>

void ms3d_vertex_to_infl(const ms3d_vertex_t *vert, mdl_influence_t *infl)
{
	infl->bone_id[0] = vert->bone_id < 0 ? MDL_INFL_BREAK : vert->bone_id;

	if(vert->weight[0] != 0)
	{
		uchar i;

		infl->weight[0] = (float)vert->weight[0] * 0.01f;
		infl->bone_id[1] = vert->bone_ids[0] < 0 ? MDL_INFL_BREAK : vert->bone_ids[0];
		infl->bone_id[2] = vert->bone_ids[1] < 0 ? MDL_INFL_BREAK : vert->bone_ids[1];
		infl->bone_id[3] = vert->bone_ids[2] < 0 ? MDL_INFL_BREAK : vert->bone_ids[2];

		for(i = 1; i < 3; i++)
		{
			if(vert->weight[i] == 0)
			{
				infl->bone_id[i] = MDL_INFL_BREAK;
				break;
			}
			else
			{
				infl->weight[i] = (float)vert->weight[i] * 0.01f;
			}
		}

		if(i == 3)
		{
			if(vert->weight[0] + vert->weight[1] + vert->weight[2] >= 100)
			{
				infl->bone_id[3] = MDL_INFL_BREAK;
			}
		}
	}
	else
	{
		infl->weight[0] = 1.0;
		infl->bone_id[1] = MDL_INFL_BREAK;
	}
}

Bool ms3d_to_mdl_mesh_pre(const ms3d_model_t *ms3d,
						  mdl_mesh_t *mesh)
{
	if(ms3d && mesh)
	{
		ushort i, j, k;
		ushort vcount = 0;
		uint icount = 0;
		// vmap[ms3d顶点ID] = mesh顶点ID
		Bool *vmap = (Bool*)ut_alloc(sizeof(Bool) * ms3d->numVerts);

		// 清空 vmap
		memset(vmap, 0, sizeof(Bool) * ms3d->numVerts);

		// 遍历组
		for(i = 0; i < ms3d->numGroups; i++)
		{
			ms3d_group_t *group = &ms3d->groups[i];

			// 遍历三角形
			for(j = 0; j < group->group_triangles; j++)
			{
				ushort tid = group->tri_id[j];
				ms3d_triangle_t *triangle = &ms3d->tris[tid];

				// 三个顶点
				for(k = 0; k < 3; k++)
				{
					ushort indi = triangle->vertex_id[k];

					// 当前顶点没有被写入过
					if(vmap[indi] == 0)
					{
						vmap[indi] = 1;
						vcount++;
					}
				}
			}

			icount += group->group_triangles * 3;

		} // 遍历组

		ut_free(vmap);

		// 申请空间
		mesh->vert_count = vcount;
		mesh->indi_count = icount;

		mesh->vertices = (vec3*)ut_alloc(sizeof(vec3) * vcount);
		mesh->texcoords = (vec2*)ut_alloc(sizeof(vec2) * vcount);
		mesh->normals = (vec3*)ut_alloc(sizeof(vec3) * vcount);
		memset(mesh->normals, 0, sizeof(vec3) * vcount);
		//! mesh->tangents = ut_alloc(sizeof(vec3) * vcount);
		mesh->influences = (mdl_influence_t*)ut_alloc(sizeof(mdl_influence_t) * vcount);

		mesh->indices = (ushort*)ut_alloc(sizeof(ushort) * icount);

		mesh->group_count = ms3d->numGroups;
		mesh->groups = (mdl_group_t*)ut_alloc(sizeof(mdl_group_t) * mesh->group_count);

		return True;
	}
	return False;
}

Bool ms3d_to_mdl_mesh(const ms3d_model_t *ms3d,
					  mdl_mesh_t *mesh)
{
	if(ms3d && mesh)
	{
		ushort i, j, k, id;
		ushort vcount = 0;
		uint icount = 0;
		// vmap[ms3d顶点ID] = mesh顶点ID
		ushort *vmap = (ushort*)ut_alloc(sizeof(ushort) * ms3d->numVerts);

		// 清空 vmap
		for(j = 0; j < ms3d->numVerts; j++)
			vmap[j] = (ushort)65535;

		// 遍历组
		for(i = 0; i < ms3d->numGroups; i++)
		{
			ms3d_group_t *group = &ms3d->groups[i];

			mesh->groups[i].indi_begin = icount;

			// 遍历三角形
			for(j = 0; j < group->group_triangles; j++)
			{
				ushort tid = group->tri_id[j];
				ms3d_triangle_t *triangle = &ms3d->tris[tid];

				// 三个顶点
				for(k = 0; k < 3; k++)
				{
					ushort indi = triangle->vertex_id[k];
					ms3d_vertex_t *vert = &ms3d->vertices[indi];

					// 当前顶点没有被写入过
					if(vmap[indi] == (ushort)65535)
					{
						vmap[indi] = vcount;
						vcount++;

						id = vmap[indi];

						// 复制顶点
						memmove(&mesh->vertices[id], vert->vertex_pos, sizeof(vec3));
						// 镜像 Z 轴
						mesh->vertices[id][2] = -mesh->vertices[id][2];

						// 复制纹理坐标
						mesh->texcoords[id][0] = triangle->s[k];
						mesh->texcoords[id][1] = 1.0f - triangle->t[k];

						// 复制顶点权重
						if(mesh->influences)
							ms3d_vertex_to_infl(vert, &mesh->influences[id]);
						
					}

					id = vmap[indi];

					// 叠加法线
					//! 注意最终要 normalize
					mesh->normals[id][0] += triangle->vertex_normal[k * 3 + 0];
					mesh->normals[id][1] += triangle->vertex_normal[k * 3 + 1];
					mesh->normals[id][2] += triangle->vertex_normal[k * 3 + 2];

					// 记录当前索引
					mesh->indices[icount] = id;
					icount++;
				}
			}

			mesh->groups[i].indi_len = icount - mesh->groups[i].indi_begin;
			mesh->groups[i].material_id = group->material_id;
		} // 遍历组

		ut_free(vmap);

		// 单位化法线
		for(i = 0; i < mesh->vert_count; i++)
		{
			// 镜像法线
			mesh->normals[i][2] = -mesh->normals[i][2];
			vecNormalize(mesh->normals[i], mesh->normals[i]);
		}

		//! mesh->indi_count == icount;
		//! mesh->vert_count == vcount;

		return True;
	}
	return False;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
int ms3d_find_bone(const ms3d_model_t *ms3d, char parent_name[MS3D_MAX_NAME_SIZE])
{
	if(!parent_name || parent_name[0] == 0) return -1;
	else
	{
		ushort i;
		for(i = 0; i < ms3d->numJoints; i++)
		{
			if(!strcmp(ms3d->joints[i].name, parent_name))
				return i;
		}
	}
	return -1;
}

Bool ms3d_to_mdl_skel_pre(const ms3d_model_t *ms3d, mdl_skeleton_t *skel)
{
	if(ms3d && skel)
	{
		skel->bone_count = ms3d->numJoints;
		skel->bones = (mdl_bone_t*)ut_alloc(sizeof(mdl_bone_t) * skel->bone_count);
		return True;
	}
	return False;
}

Bool ms3d_to_mdl_skel(const ms3d_model_t *ms3d, mdl_skeleton_t *skel)
{
	if(ms3d && skel)
	{
		ushort i;
		for(i = 0; i < skel->bone_count; i++)
		{
			// 转换为四元数
			quaAngleR(ms3d->joints[i].rot, skel->bones[i].key.rot);
			// 镜像
			skel->bones[i].key.rot[0] = -skel->bones[i].key.rot[0];
			skel->bones[i].key.rot[1] = -skel->bones[i].key.rot[1];

			memmove(skel->bones[i].key.pos, ms3d->joints[i].pos, sizeof(vec3));
			// 镜像
			skel->bones[i].key.pos[2] = -skel->bones[i].key.pos[2];

			skel->bones[i].parent_id = ms3d_find_bone(ms3d, ms3d->joints[i].parent_name);
		}
		return True;
	}
	return False;
}

Bool ms3d_to_mdl_track_pre(const ms3d_model_t *ms3d, mdl_track_t *track,
						   const ushort *bone_aff_ids, ushort bone_aff_count)
{
	if(ms3d && track)
	{
		ushort i, j;
		uint koff = 0;
		ms3d_joint_t *joints = ms3d->joints;

		// 如果包含用户自定义骨骼影响
		if(bone_aff_ids && bone_aff_count)
		{
			// 复制到动画链内
			track->bone_aff_count = bone_aff_count;
			track->bone_aff_ids = (ushort*)ut_alloc(sizeof(ushort) * bone_aff_count);
			memmove(track->bone_aff_ids, bone_aff_ids, sizeof(ushort) * bone_aff_count);
		}
		else
		{
			// 否则根据 ms3d 初始化
			track->bone_aff_count = ms3d->numJoints;
			track->bone_aff_ids = (ushort*)ut_alloc(sizeof(ushort) * track->bone_aff_count);

			// 初始化骨骼影响索引, 从 0 开始顺序排列
			for(i = 0; i < track->bone_aff_count; i++)
				track->bone_aff_ids[i] = i;
		}

		// 检查动画信息是否符合要求
		for(i = 0; i < track->bone_aff_count; i++)
		{
			ushort idx = track->bone_aff_ids[i];

			if(joints[idx].rot_count != joints[idx].pos_count)
			{
				printf("track error: keyframe numbers not equal!\n");
				return False;
			}

			for(j = 0; j < joints[idx].rot_count; j++)
			{
				if(joints[idx].rotates[j].time != joints[idx].translates[j].time)
				{
					printf("track error: keyframe time not equal!\n");
					return False;
				}
			}

			// 累计帧个数
			koff += joints[idx].rot_count;
		}

		track->keyfrm_count = koff;
		track->keyfrm_pool = (mdl_keyframe_t*)ut_alloc(sizeof(mdl_keyframe_t) * koff);

		track->tracks = (mdl_frames_t*)ut_alloc(sizeof(mdl_frames_t) * track->bone_aff_count);
		
		return True;
	}
	return False;
}

Bool ms3d_to_mdl_track(const ms3d_model_t *ms3d, mdl_track_t *track)
{
	if(ms3d && track)
	{
		ushort i, j;
		uint koff = 0;
		ms3d_joint_t *joints = ms3d->joints;

		for(i = 0; i < track->bone_aff_count; i++)
		{
			ushort idx = track->bone_aff_ids[i];

			track->tracks[i].frames = track->keyfrm_pool + koff;
			track->tracks[i].frm_count = joints[idx].rot_count;

			for(j = 0; j < joints[idx].rot_count; j++)
			{
				track->keyfrm_pool[koff].time = joints[idx].rotates[j].time;

				// 转换为四元数
				quaAngleR(joints[idx].rotates[j].key, track->keyfrm_pool[koff].key.rot);
				// 镜像
				track->keyfrm_pool[koff].key.rot[0] = -track->keyfrm_pool[koff].key.rot[0];
				track->keyfrm_pool[koff].key.rot[1] = -track->keyfrm_pool[koff].key.rot[1];

				memmove(track->keyfrm_pool[koff].key.pos, joints[idx].translates[j].key, sizeof(vec3));
				// 镜像
				track->keyfrm_pool[koff].key.pos[2] = -track->keyfrm_pool[koff].key.pos[2];

				koff++;
			}
		}
		return True;
	}
	return False;
}
