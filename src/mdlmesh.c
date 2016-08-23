// 110126
// 110129 ��ӷ����ںϺ���

#include "mdlcalc.h"
#include "ut.h"
#include "factory.h"
#include <string.h>

extern factory_t g_factory_mdl_mesh;

#define mdl_fac_mesh_yield			((mdl_mesh_t*)factory_yield(&g_factory_mdl_mesh))
#define mdl_fac_mesh_recycle(_p)	(factory_recycle(&g_factory_mdl_mesh, _p))

//////////////////////////////////////////////////////////////////////////
// ���������ںϼ���
void mdl_vertex_blend(const vec3 vert, const mdl_influence_t *infl,
					  const mat4 *mat_global_anim_list,
					  vec3 vert_out)
{
	uchar i;
	vec3 tmp;

	for(i = 0; i < 4; i++)
	{
		if(infl->bone_id[i] == MDL_INFL_BREAK) break;
		else if(i == 0) memset(vert_out, 0, sizeof(vec3));

		// ��������
		vecTransform(vert, mat_global_anim_list[infl->bone_id[i]], tmp);

		if(i == 3)
		{
			float weight = 1.0f - (infl->weight[0] + infl->weight[1] + infl->weight[2]);

			if(weight > 0.0)
			{
				vert_out[0] += tmp[0] * weight;
				vert_out[1] += tmp[1] * weight;
				vert_out[2] += tmp[2] * weight;
			}
		}
		else
		{
			vert_out[0] += tmp[0] * infl->weight[i];
			vert_out[1] += tmp[1] * infl->weight[i];
			vert_out[2] += tmp[2] * infl->weight[i];
		}
	}
}

// ���������ںϼ���
void mdl_normal_blend(const vec3 norm, const mdl_influence_t *infl,
					  const mat4 *mat_global_anim_list,
					  vec3 norm_out)
{
	uchar i;
	vec3 tmp;

	for(i = 0; i < 4; i++)
	{
		if(infl->bone_id[i] == MDL_INFL_BREAK) break;
		else if(i == 0) memset(norm_out, 0, sizeof(vec3));

		// ��������
		vecRotate(norm, mat_global_anim_list[infl->bone_id[i]], tmp);

		if(i == 3)
		{
			float weight = 1.0f - (infl->weight[0] + infl->weight[1] + infl->weight[2]);

			if(weight > 0.0)
			{
				norm_out[0] += tmp[0] * weight;
				norm_out[1] += tmp[1] * weight;
				norm_out[2] += tmp[2] * weight;
			}
		}
		else
		{
			norm_out[0] += tmp[0] * infl->weight[i];
			norm_out[1] += tmp[1] * infl->weight[i];
			norm_out[2] += tmp[2] * infl->weight[i];
		}
	}
}

Bool mdl_mesh_create(mdl_mesh_t **mesh)
{
	if(mesh)
	{
		// �����ṹ, ���Դӹ���������һ���ṹ
		*mesh = mdl_fac_mesh_yield;

		// ��ʱ������д�ṹ�е�����
		memset(*mesh, 0, sizeof(mdl_mesh_t));

		// ��ʼ�����ü���Ϊ0
		refcount_init(*mesh);

		// ���һ������
		reference(*mesh);

		return True;
	}
	return False;
}

void mdl_mesh_release(mdl_mesh_t **mesh)
{
	if(mesh)
	{
		// �������Ϊ0
		if(reference_release(*mesh))
		{
			// �ͷų�Ա����
			{
				ut_free((*mesh)->vertices);
				ut_free((*mesh)->texcoords);
				ut_free((*mesh)->normals);
				ut_free((*mesh)->tangents);
				ut_free((*mesh)->binormals);
				ut_free((*mesh)->influences);
				ut_free((*mesh)->indices);
				ut_free((*mesh)->groups);
			}

			// �ͷŽṹʵ��, �����ù�������
			mdl_fac_mesh_recycle(*mesh);
		}
		*mesh = 0;
	}
}

Bool mdl_mesh_blend(const mdl_mesh_t *mesh, const mat4 *mat_global_anim_list,
					vec3 *vertices_out, vec3 *normals_out, vec3 *tangents_out, vec3 *binormals_out)
{
	if(mesh && mat_global_anim_list)
	{
		ushort i;

		if(vertices_out && mesh->vertices)
		{
			for(i = 0; i < mesh->vert_count; i++)
			{
				mdl_vertex_blend(mesh->vertices[i], &mesh->influences[i],
					mat_global_anim_list,
					vertices_out[i]);
			}
		}

		if(normals_out && mesh->normals)
		{
			for(i = 0; i < mesh->vert_count; i++)
			{
				mdl_normal_blend(mesh->normals[i], &mesh->influences[i],
					mat_global_anim_list,
					normals_out[i]);
			}
		}

		if(tangents_out && mesh->tangents)
		{
			for(i = 0; i < mesh->vert_count; i++)
			{
				mdl_normal_blend(mesh->tangents[i], &mesh->influences[i],
					mat_global_anim_list,
					tangents_out[i]);
			}
		}

		if(binormals_out && mesh->binormals)
		{
			for(i = 0; i < mesh->vert_count; i++)
			{
				mdl_normal_blend(mesh->binormals[i], &mesh->influences[i],
					mat_global_anim_list,
					binormals_out[i]);
			}
		}

		return True;
	}
	return False;
}

// GPU �����ں�ʱ, ����ȡ������Ȩ����
Bool mdl_mesh_infl_stream(const mdl_mesh_t *mesh, ushort4 **bone_ids, vec4 **bone_weights)
{
	if(mesh && bone_ids && bone_weights)
	{
		ushort4 *bindices = (ushort4*)ut_alloc(sizeof(ushort4) * mesh->vert_count);
		vec4 *bweights = (vec4*)ut_alloc(sizeof(vec4) * mesh->vert_count);
		ushort i, j;

		memset(bindices, 0, sizeof(ushort4) * mesh->vert_count);
		memset(bweights, 0, sizeof(vec4) * mesh->vert_count);

		for(i = 0; i < mesh->vert_count; i++)
		{
			mdl_influence_t *infl = &mesh->influences[i];
			for(j = 0; j < 4; j++)
			{
				if(infl->bone_id[j] == MDL_INFL_BREAK) break;
				bindices[i][j] = infl->bone_id[j];

				if(j == 3)
				{
					float weight = 1.0f - (infl->weight[0] + infl->weight[1] + infl->weight[2]);
					bweights[i][j] = weight;
				}
				else
				{
					bweights[i][j] = infl->weight[j];
				}
			}
		}

		*bone_ids = bindices;
		*bone_weights = bweights;

		return True;
	}
	return False;
}

void mdl_mesh_infl_stream_free(ushort4 *bone_ids, vec4 *bone_weights)
{
	ut_free(bone_ids);
	ut_free(bone_weights);
}

//////////////////////////////////////////////////////////////////////////
// ֻҪ�ж������������, �Ϳ�������ߺ������б�
Bool mdl_mesh_calc_tangent(mdl_mesh_t *mesh, Bool b_normal, Bool b_tangent, Bool b_binormal)
{
	if(mesh && mesh->vertices && mesh->texcoords && (b_normal || b_tangent || b_binormal))
	{
		ushort i;

		if(b_normal)
		{
			// ��������ڷ����б�, �������ڴ�
			if(!mesh->normals)
			{
				mesh->normals = (vec3*)ut_alloc(sizeof(vec3) * mesh->vert_count);
			}

			// ��Ҫ���ԭ������
			memset(mesh->normals, 0, sizeof(vec3) * mesh->vert_count);
		}

		if(b_tangent)
		{
			// ��������������б�, �������ڴ�
			if(!mesh->tangents)
			{
				mesh->tangents = (vec3*)ut_alloc(sizeof(vec3) * mesh->vert_count);
			}

			// ��Ҫ���ԭ������
			memset(mesh->tangents, 0, sizeof(vec3) * mesh->vert_count);
		}

		if(b_binormal)
		{
			// ��������������б�, �������ڴ�
			if(!mesh->binormals)
			{
				mesh->binormals = (vec3*)ut_alloc(sizeof(vec3) * mesh->vert_count);
			}

			// ��Ҫ���ԭ������
			memset(mesh->binormals, 0, sizeof(vec3) * mesh->vert_count);
		}

		// ����������
		for(i = 0; i < mesh->group_count; i++)
		{
			mdl_group_t *group = &mesh->groups[i];
			uint j;

			// ������������
			for(j = group->indi_begin; j < group->indi_begin + group->indi_len; j += 3)
			{
				// һ�������������
				ushort index0 = mesh->indices[j];
				ushort index1 = mesh->indices[j + 1];
				ushort index2 = mesh->indices[j + 2];

				vec3 onormal, otangent, obinormal;

				// ���ݶ����Լ�����˳��(�����涥��˳��)
				// �������, ����, ������
				tangentSpaceLH(mesh->vertices[index0],
					mesh->vertices[index1],
					mesh->vertices[index2],
					mesh->texcoords[index0],
					mesh->texcoords[index1],
					mesh->texcoords[index2],
					onormal, otangent, obinormal);

				if(b_normal)
				{
					// �����·���
					vecAdd3(onormal, mesh->normals[index0], mesh->normals[index0]);
					vecAdd3(onormal, mesh->normals[index1], mesh->normals[index1]);
					vecAdd3(onormal, mesh->normals[index2], mesh->normals[index2]);
				}
				if(b_tangent)
				{
					// ����������
					vecAdd3(otangent, mesh->tangents[index0], mesh->tangents[index0]);
					vecAdd3(otangent, mesh->tangents[index1], mesh->tangents[index1]);
					vecAdd3(otangent, mesh->tangents[index2], mesh->tangents[index2]);
				}
				if(b_binormal)
				{
					// �����¸�����
					vecAdd3(obinormal, mesh->binormals[index0], mesh->binormals[index0]);
					vecAdd3(obinormal, mesh->binormals[index1], mesh->binormals[index1]);
					vecAdd3(obinormal, mesh->binormals[index2], mesh->binormals[index2]);
				}
			}
		}

		// ���Ҫȫ����λ��
		//! ��� GPU �������� normalize ���Ҳ���Ҫ���浽�ļ�, ��Ҳ�ɲ��õ�λ��
		for(i = 0; i < mesh->vert_count; i++)
		{
			if(b_normal) vecNormalize(mesh->normals[i], mesh->normals[i]);
			if(b_tangent) vecNormalize(mesh->tangents[i], mesh->tangents[i]);
			if(b_binormal) vecNormalize(mesh->binormals[i], mesh->binormals[i]);
		}

		return True;
	}
	return False;
}
