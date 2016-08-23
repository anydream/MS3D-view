// 110126

#include "mdlcalc.h"
#include "ut.h"
#include "factory.h"
#include <string.h>

extern factory_t g_factory_mdl_skeleton;

#define mdl_fac_skel_yield			((mdl_skeleton_t*)factory_yield(&g_factory_mdl_skeleton))
#define mdl_fac_skel_recycle(_p)	(factory_recycle(&g_factory_mdl_skeleton, _p))

//////////////////////////////////////////////////////////////////////////
// 函数: 旋转/位移插值两个键
// 返回插值后的旋转四元数/位置
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
		// 创建结构, 可以从工厂中生产一个结构
		*skel = mdl_fac_skel_yield;

		// 此时可以填写结构中的数据
		memset(*skel, 0, sizeof(mdl_skeleton_t));

		// 初始化引用计数为0
		refcount_init(*skel);

		// 添加一次引用
		reference(*skel);

		return True;
	}
	return False;
}

void mdl_skel_release(mdl_skeleton_t **skel)
{
	if(skel)
	{
		// 如果引用为0
		if(reference_release(*skel))
		{
			// 释放成员数据
			{
				ut_free((*skel)->bones);
			}

			// 释放结构实体, 可以让工厂回收
			mdl_fac_skel_recycle(*skel);
		}
		*skel = 0;
	}
}

//////////////////////////////////////////////////////////////////////////
// 函数: 获得骨架的自身和全局骨骼矩阵
// 根据一个骨架模板, 以及一个父矩阵, 计算出所有骨骼的自身和全局矩阵的逆矩阵(标准姿势)
// 输出两种矩阵列表: 自身和全局矩阵. 前者用于动画, 后者用于逆运算顶点
// 注意: 一个骨架只需调用一次!
Bool mdl_skel_update(const mdl_skeleton_t *skel,
					 mat4 *mat_local_skel_list, mat4 *mat_iglobal_skel_list)
{
	if(skel && mat_local_skel_list && mat_iglobal_skel_list)
	{
		ushort i;

		// 遍历骨架中所有骨骼
		for(i = 0; i < skel->bone_count; i++)
		{
			// 当前骨骼的父骨骼下标
			int pid = skel->bones[i].parent_id;
			// 当前骨骼的旋转和位移数据, 转换成变换矩阵
			mdl_key_matrix(skel->bones[i].key, mat_local_skel_list[i]);

			// 如果是根骨骼, 则直接作为世界矩阵
			if(pid == -1) memmove(mat_iglobal_skel_list[i], mat_local_skel_list[i], sizeof(mat4));
			// 否则乘上其父矩阵
			else matMul(mat_local_skel_list[i], mat_iglobal_skel_list[pid], mat_iglobal_skel_list[i]);
		}

		// 然后求逆矩阵
		for(i = 0; i < skel->bone_count; i++)
		{
			matInverse(mat_iglobal_skel_list[i], mat_iglobal_skel_list[i]);
		}
		return True;
	}
	return False;
}

// 函数: 获得经过动画的骨架的最终矩阵列表
// 输入骨骼的自身矩阵列表, 标准姿势逆矩阵表, 以及动画矩阵列表
// 输出动画后的骨骼世界矩阵列表(最终矩阵列表)
// 附加: mat_local_anim_list 作为临时存储列表
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

		// 遍历骨架中所有骨骼
		for(i = 0; i < skel->bone_count; i++)
		{
			// 当前骨骼的父骨骼下标
			int pid = skel->bones[i].parent_id;

			matMul(mat_animate_list[i], mat_local_skel_list[i], mat_local_anim_list[i]);

			if(pid == -1) memmove(mat_global_anim_list[i], mat_local_anim_list[i], sizeof(mat4));
			else matMul(mat_local_anim_list[i], mat_global_anim_list[pid], mat_global_anim_list[i]);
		}

		// 然后乘上世界骨骼的逆矩阵
		for(i = 0; i < skel->bone_count; i++)
		{
			matMul(mat_iglobal_skel_list[i], mat_global_anim_list[i], mat_global_anim_list[i]);
		}
		return True;
	}
	return False;
}

// GPU 顶点融合时, 用于把二维矩阵列表转换为一维矩阵列表
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
