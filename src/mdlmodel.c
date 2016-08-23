// 110123
// 110127

#include "mdlcalc.h"
#include "ut.h"
#include "factory.h"
#include <string.h>

extern factory_t g_factory_mdl_node;

#define mdl_fac_node_yield			((mdl_node_t*)factory_yield(&g_factory_mdl_node))
#define mdl_fac_node_recycle(_p)	(factory_recycle(&g_factory_mdl_node, _p))

//////////////////////////////////////////////////////////////////////////
// ��һ�����������ں���
Bool mdl_mesh_blender_bind(mdl_mesh_blender_t *blender, mdl_mesh_t *mesh)
{
	if(blender && mesh && mesh != blender->mesh)
	{
		ushort vert_count = mesh->vert_count;

		if(blender->mesh)
		{
			if(blender->mesh->vert_count != vert_count)
			{
				blender->vertex_stream = ut_realloc(blender->vertex_stream, sizeof(vec3) * vert_count);
				blender->normal_stream = ut_realloc(blender->normal_stream, sizeof(vec3) * vert_count);
				blender->tangent_stream = ut_realloc(blender->tangent_stream, sizeof(vec3) * vert_count);
				blender->binormal_stream = ut_realloc(blender->binormal_stream, sizeof(vec3) * vert_count);
			}

			mdl_mesh_release(&blender->mesh);
		}
		else
		{
			blender->vertex_stream = ut_alloc(sizeof(vec3) * vert_count);
			blender->normal_stream = ut_alloc(sizeof(vec3) * vert_count);
			blender->tangent_stream = ut_alloc(sizeof(vec3) * vert_count);
			blender->binormal_stream = ut_alloc(sizeof(vec3) * vert_count);
		}

		// ���µ� mesh ��Ķ���ͷ��ߵ����ݸ��Ƶ�����
		memmove(blender->vertex_stream, mesh->vertices, sizeof(vec3) * vert_count);
		memmove(blender->normal_stream, mesh->normals, sizeof(vec3) * vert_count);
		memmove(blender->tangent_stream, mesh->tangents, sizeof(vec3) * vert_count);
		memmove(blender->binormal_stream, mesh->binormals, sizeof(vec3) * vert_count);

		blender->mesh = reference(mesh);

		return True;
	}
	return False;
}

// ȡ����һ���ں����������
//! Ӧ�����ٵ����������
Bool mdl_mesh_blender_release(mdl_mesh_blender_t *blender)
{
	if(blender && blender->mesh)
	{
		ut_free(blender->vertex_stream);
		ut_free(blender->normal_stream);
		ut_free(blender->tangent_stream);
		ut_free(blender->binormal_stream);

		mdl_mesh_release(&blender->mesh);
		return True;
	}
	return False;
}

// ����һ���ں�
Bool mdl_mesh_blender_blend(mdl_mesh_blender_t *blender, const mat4 *mat_global_anim_list)
{
	if(blender && blender->mesh)
	{
		// ������ں�
		return mdl_mesh_blend(blender->mesh, mat_global_anim_list,
			blender->vertex_stream,
			blender->normal_stream,
			blender->tangent_stream,
			blender->binormal_stream);
	}
	return False;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
Bool mdl_node_create(mdl_node_t **node)
{
	if(node)
	{
		// �����ṹ, ���Դӹ���������һ���ṹ
		*node = mdl_fac_node_yield;

		// ��ʱ������д�ṹ�е�����
		memset(*node, 0, sizeof(mdl_node_t));

		// ��ʼ�����ü���Ϊ0
		refcount_init(*node);

		// ���һ������
		reference(*node);

		return True;
	}
	return False;
}

void mdl_node_release(mdl_node_t **node)
{
	if(node)
	{
		// �������Ϊ0
		if(reference_release(*node))
		{
			// �ͷų�Ա����
			ushort i;

			mdl_node_release(&(*node)->first_child);
			mdl_node_release(&(*node)->next_sibling);

			mdl_node_release_skeleton(*node);

			for(i = 0; i < (*node)->anim_ctrl_count; i++)
			{
				mdl_anim_release(&(*node)->anim_ctrl[i]);
			}
			ut_free((*node)->anim_ctrl);

			for(i = 0; i < (*node)->mesh_blender_count; i++)
			{
				mdl_mesh_blender_release(&(*node)->mesh_blender[i]);
			}
			ut_free((*node)->mesh_blender);

			// �ͷŽṹʵ��, �����ù�������
			mdl_fac_node_recycle(*node);
		}
		*node = 0;
	}
}

Bool mdl_node_bind_skeleton(mdl_node_t *node, mdl_skeleton_t *skel)
{
	if(node && skel && skel != node->skeleton)
	{
		ushort bone_count = skel->bone_count;

		if(node->skeleton)
		{
			if(node->skeleton->bone_count != bone_count)
			{
				node->mat_iglobal_skel = ut_realloc(node->mat_iglobal_skel, sizeof(mat4) * bone_count);
				node->mat_global_anim = ut_realloc(node->mat_global_anim, sizeof(mat4) * bone_count);
				node->mat_local_skel = ut_realloc(node->mat_local_skel, sizeof(mat4) * bone_count);
				node->mat_local_anim = ut_realloc(node->mat_local_anim, sizeof(mat4) * bone_count);
				node->mat_anim_list = ut_realloc(node->mat_anim_list, sizeof(mat4) * bone_count);
			}

			mdl_skel_release(&node->skeleton);
		}
		else
		{
			node->mat_iglobal_skel = ut_alloc(sizeof(mat4) * bone_count);
			node->mat_global_anim = ut_alloc(sizeof(mat4) * bone_count);
			node->mat_local_skel = ut_alloc(sizeof(mat4) * bone_count);
			node->mat_local_anim = ut_alloc(sizeof(mat4) * bone_count);
			node->mat_anim_list = ut_alloc(sizeof(mat4) * bone_count);
		}

		memset(node->mat_global_anim, 0, sizeof(mat4) * bone_count);
		memset(node->mat_local_anim, 0, sizeof(mat4) * bone_count);
		memset(node->mat_anim_list, 0, sizeof(mat4) * bone_count);

		node->skeleton = reference(skel);

		// ����һ�ιǼܱ�׼���ƾ���
		mdl_skel_update(node->skeleton, node->mat_local_skel, node->mat_iglobal_skel);

		return True;
	}
	return False;
}

Bool mdl_node_release_skeleton(mdl_node_t *node)
{
	if(node && node->skeleton)
	{
		ut_free(node->mat_iglobal_skel);
		ut_free(node->mat_global_anim);
		ut_free(node->mat_local_skel);
		ut_free(node->mat_local_anim);
		ut_free(node->mat_anim_list);

		mdl_skel_release(&node->skeleton);
		return True;
	}
	return False;
}

Bool mdl_node_bind_animate(mdl_node_t *node, mdl_animate_t *anim)
{
	if(node && anim)
	{
		node->anim_ctrl = ut_realloc(node->anim_ctrl,
			sizeof(mdl_animate_t*) * node->anim_ctrl_count + 1);
		node->anim_ctrl[node->anim_ctrl_count] = reference(anim);
		node->anim_ctrl_count++;
		return True;
	}
	return False;
}

Bool mdl_node_bind_mesh(mdl_node_t *node, mdl_mesh_t *mesh)
{
	if(node && mesh)
	{
		Bool res;

		node->mesh_blender = ut_realloc(node->mesh_blender,
			sizeof(mdl_mesh_blender_t) * node->mesh_blender_count + 1);
		memset(&node->mesh_blender[node->mesh_blender_count], 0, sizeof(mdl_mesh_blender_t));

		res = mdl_mesh_blender_bind(&node->mesh_blender[node->mesh_blender_count], mesh);
		node->mesh_blender_count++;

		return res;
	}
	return False;
}

//////////////////////////////////////////////////////////////////////////
Bool mdl_node_advance_impl(mdl_node_t *node, float delta)
{
	if(node)
	{
		Bool res = True;
		ushort i;

		for(i = 0; i < node->anim_ctrl_count; i++)
		{
			res = mdl_anim_advance(node->anim_ctrl[i], delta) && res;
		}
		return res;
	}
	return False;
}

void mdl_node_advance(mdl_node_t *node, float delta)
{
	if(node)
	{
		mdl_node_advance_impl(node, delta);

		if(node->next_sibling) mdl_node_advance(node->next_sibling, delta);
		if(node->first_child) mdl_node_advance(node->first_child, delta);
	}
}

//! ���ڵ�����ӹ�������, Ӧ����Ϊ��Ⱦʱ�� world matrix �������붥�����
Bool mdl_node_interp_impl(mdl_node_t *node)
{
	if(node)
	{
		Bool res = True;
		ushort i;

		for(i = 0; i < node->anim_ctrl_count; i++)
		{
			res = mdl_anim_interp(node->anim_ctrl[i]) && res;
			res = mdl_anim_matrix(node->anim_ctrl[i], node->mat_anim_list) && res;
		}
		// �������ձ任����
		res = mdl_skel_animate(node->skeleton, node->mat_local_skel, node->mat_iglobal_skel,
			node->mat_anim_list, node->mat_local_anim, node->mat_global_anim) && res;

		return res;
	}
	return False;
}

void mdl_node_interp(mdl_node_t *node)
{
	if(node)
	{
		mdl_node_interp_impl(node);

		if(node->next_sibling) mdl_node_interp(node->next_sibling);
		if(node->first_child) mdl_node_interp(node->first_child);
	}
}

Bool mdl_node_blend_impl(mdl_node_t *node)
{
	if(node)
	{
		Bool res = True;
		ushort i;

		for(i = 0; i < node->mesh_blender_count; i++)
		{
			res = mdl_mesh_blender_blend(&node->mesh_blender[i], node->mat_global_anim) && res;
		}
		return res;
	}
	return False;
}

void mdl_node_blend(mdl_node_t *node)
{
	if(node)
	{
		mdl_node_blend_impl(node);

		if(node->next_sibling) mdl_node_blend(node->next_sibling);
		if(node->first_child) mdl_node_blend(node->first_child);
	}
}