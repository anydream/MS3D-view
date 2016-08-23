// 110126

#include "mdlcalc.h"
#include "ut.h"
#include "factory.h"
#include <string.h>

extern factory_t g_factory_mdl_skeleton;

#define mdl_fac_skel_yield			((mdl_skeleton_t*)factory_yield(&g_factory_mdl_skeleton))
#define mdl_fac_skel_recycle(_p)	(factory_recycle(&g_factory_mdl_skeleton, _p))

//////////////////////////////////////////////////////////////////////////
// ����: ��ת/λ�Ʋ�ֵ������
// ���ز�ֵ�����ת��Ԫ��/λ��
Bool mdl_key_interp(const mdl_key_t *k1, const mdl_key_t *k2, float weight, mdl_key_t *key_out)
{
	if(k1 && k2 && key_out)
	{
		if(k1 == k2 || weight == 0.0)
			memmove(key_out, k1, sizeof(mdl_key_t));
		else if(weight == 1.0)
			memmove(key_out, k2, sizeof(mdl_key_t));
		else
		{
			quaSlerp(k1->rot, k2->rot, weight, key_out->rot);
			vecHermite(k1->pos, k2->pos, weight, key_out->pos);
		}

		return True;
	}
	return False;
}

Bool mdl_skel_create(mdl_skeleton_t **skel)
{
	if(skel)
	{
		// �����ṹ, ���Դӹ���������һ���ṹ
		*skel = mdl_fac_skel_yield;

		// ��ʱ������д�ṹ�е�����
		memset(*skel, 0, sizeof(mdl_skeleton_t));

		// ��ʼ�����ü���Ϊ0
		refcount_init(*skel);

		// ���һ������
		reference(*skel);

		return True;
	}
	return False;
}

void mdl_skel_release(mdl_skeleton_t **skel)
{
	if(skel)
	{
		// �������Ϊ0
		if(reference_release(*skel))
		{
			// �ͷų�Ա����
			{
				ut_free((*skel)->bones);
			}

			// �ͷŽṹʵ��, �����ù�������
			mdl_fac_skel_recycle(*skel);
		}
		*skel = 0;
	}
}

//////////////////////////////////////////////////////////////////////////
// ����: ��ùǼܵ������ȫ�ֹ�������
// ����һ���Ǽ�ģ��, �Լ�һ��������, ��������й����������ȫ�־���������(��׼����)
// ������־����б�: �����ȫ�־���. ǰ�����ڶ���, �������������㶥��
// ע��: һ���Ǽ�ֻ�����һ��!
Bool mdl_skel_update(const mdl_skeleton_t *skel,
					 mat4 *mat_local_skel_list, mat4 *mat_iglobal_skel_list)
{
	if(skel && mat_local_skel_list && mat_iglobal_skel_list)
	{
		ushort i;

		// �����Ǽ������й���
		for(i = 0; i < skel->bone_count; i++)
		{
			// ��ǰ�����ĸ������±�
			int pid = skel->bones[i].parent_id;
			// ��ǰ��������ת��λ������, ת���ɱ任����
			mdl_key_matrix(skel->bones[i].key, mat_local_skel_list[i]);

			// ����Ǹ�����, ��ֱ����Ϊ�������
			if(pid == -1) memmove(mat_iglobal_skel_list[i], mat_local_skel_list[i], sizeof(mat4));
			// ��������丸����
			else matMul(mat_local_skel_list[i], mat_iglobal_skel_list[pid], mat_iglobal_skel_list[i]);
		}

		// Ȼ���������
		for(i = 0; i < skel->bone_count; i++)
		{
			matInverse(mat_iglobal_skel_list[i], mat_iglobal_skel_list[i]);
		}
		return True;
	}
	return False;
}

// ����: ��þ��������ĹǼܵ����վ����б�
// �����������������б�, ��׼����������, �Լ����������б�
// ���������Ĺ�����������б�(���վ����б�)
// ����: mat_local_anim_list ��Ϊ��ʱ�洢�б�
Bool mdl_skel_animate(const mdl_skeleton_t *skel,
					  const mat4 *mat_local_skel_list, const mat4 *mat_iglobal_skel_list,
					  const mat4 *mat_animate_list,
					  mat4 *mat_local_anim_list, mat4 *mat_global_anim_list)
{
	if(skel && mat_local_skel_list && mat_iglobal_skel_list &&
		mat_animate_list &&
		mat_local_anim_list && mat_global_anim_list)
	{
		ushort i;

		// �����Ǽ������й���
		for(i = 0; i < skel->bone_count; i++)
		{
			// ��ǰ�����ĸ������±�
			int pid = skel->bones[i].parent_id;

			matMul(mat_animate_list[i], mat_local_skel_list[i], mat_local_anim_list[i]);

			if(pid == -1) memmove(mat_global_anim_list[i], mat_local_anim_list[i], sizeof(mat4));
			else matMul(mat_local_anim_list[i], mat_global_anim_list[pid], mat_global_anim_list[i]);
		}

		// Ȼ�������������������
		for(i = 0; i < skel->bone_count; i++)
		{
			matMul(mat_iglobal_skel_list[i], mat_global_anim_list[i], mat_global_anim_list[i]);
		}
		return True;
	}
	return False;
}

// GPU �����ں�ʱ, ���ڰѶ�ά�����б�ת��Ϊһά�����б�
Bool mdl_skel_final_matrix_array(const mdl_skeleton_t *skel, const mat4 *mat_global_anim_list,
								 float *mat_array, ushort mat_element_count)
{
	if(skel && mat_global_anim_list && mat_array && mat_element_count)
	{
		ushort i;
		for(i = 0; i < skel->bone_count && i < mat_element_count; i++)
		{
			memmove(&mat_array[16 * i], mat_global_anim_list[i], sizeof(mat4));
		}
		return True;
	}
	return False;
}
