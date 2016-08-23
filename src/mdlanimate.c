// 110126

#include "mdlcalc.h"
#include "ut.h"
#include "factory.h"
#include <string.h>

extern factory_t g_factory_mdl_animate;

#define mdl_fac_anim_yield			((mdl_animate_t*)factory_yield(&g_factory_mdl_animate))
#define mdl_fac_anim_recycle(_p)	(factory_recycle(&g_factory_mdl_animate, _p))

//////////////////////////////////////////////////////////////////////////
// 初始化动画结构
Bool mdl_anim_create(mdl_animate_t **anim)
{
	if(anim)
	{
		// 创建结构, 可以从工厂中生产一个结构
		*anim = mdl_fac_anim_yield;

		// 此时可以填写结构中的数据
		memset(*anim, 0, sizeof(mdl_animate_t));

		// 初始化引用计数为0
		refcount_init(*anim);

		// 添加一次引用
		reference(*anim);

		return True;
	}
	return False;
}

// 释放动画结构的指针成员所维持的空间
void mdl_anim_release(mdl_animate_t **anim)
{
	if(anim)
	{
		// 如果引用为0
		if(reference_release(*anim))
		{
			// 释放成员数据
			{
				ut_free((*anim)->goto_proc.start_keys);
				ut_free((*anim)->goto_proc.end_keys);
				ut_free((*anim)->target_frms);
				ut_free((*anim)->keys_interp);

				mdl_track_release(&(*anim)->track);
			}

			// 释放结构实体, 可以让工厂回收
			mdl_fac_anim_recycle(*anim);
		}
		*anim = 0;
	}
}

// 绑定一个动画链到动画, 同时申请插值列表空间, 以及用于加速寻帧的目标帧下标列表空间
Bool mdl_anim_bind_track(mdl_animate_t *anim, mdl_track_t *track)
{
	if(anim && track && track != anim->track)
	{
		ushort bone_aff_count = track->bone_aff_count;

		if(anim->track)
		{
			if(anim->track->bone_aff_count != bone_aff_count)
			{
				anim->target_frms = (uint*)ut_realloc(anim->target_frms, sizeof(uint) * bone_aff_count);
				anim->keys_interp = (mdl_key_t*)ut_realloc(anim->keys_interp, sizeof(mdl_key_t) * bone_aff_count);
				anim->goto_proc.start_keys = (mdl_key_t*)ut_realloc(anim->goto_proc.start_keys, sizeof(mdl_key_t) * bone_aff_count);
				anim->goto_proc.end_keys = (mdl_key_t*)ut_realloc(anim->goto_proc.end_keys, sizeof(mdl_key_t) * bone_aff_count);
			}

			mdl_track_release(&anim->track);
		}
		else
		{
			anim->target_frms = (uint*)ut_alloc(sizeof(uint) * bone_aff_count);
			anim->keys_interp = (mdl_key_t*)ut_alloc(sizeof(mdl_key_t) * bone_aff_count);
			anim->goto_proc.start_keys = (mdl_key_t*)ut_alloc(sizeof(mdl_key_t) * bone_aff_count);
			anim->goto_proc.end_keys = (mdl_key_t*)ut_alloc(sizeof(mdl_key_t) * bone_aff_count);
		}

		memset(anim->target_frms, 0, sizeof(uint) * bone_aff_count);
		memset(anim->keys_interp, 0, sizeof(mdl_key_t) * bone_aff_count);
		memset(anim->goto_proc.start_keys, 0, sizeof(mdl_key_t) * bone_aff_count);
		memset(anim->goto_proc.end_keys, 0, sizeof(mdl_key_t) * bone_aff_count);
		anim->b_goto = False;

		anim->track = reference(track);
		
		return True;
	}
	return False;
}

// 插值当前动画
Bool mdl_anim_interp(mdl_animate_t *anim)
{
	if(anim && anim->track)
	{
		if(anim->b_goto)
		{
			ushort i;
			for(i = 0; i < anim->track->bone_aff_count; i++)
			{
				mdl_key_interp(&anim->goto_proc.start_keys[i],
					&anim->goto_proc.end_keys[i],
					anim->goto_proc.weight,
					&anim->keys_interp[i]);
			}
			return True;
		}
		else return mdl_track_interp(anim->track, anim->time, anim->target_frms, anim->keys_interp);
	}
	return False;
}

// 把插值结果转换成矩阵, 并替换到动画矩阵列表
Bool mdl_anim_matrix(const mdl_animate_t *anim, mat4 *mat_animate_list)
{
	if(anim && anim->track && mat_animate_list)
	{
		ushort i;
		mdl_track_t *track = anim->track;

		for(i = 0; i < track->bone_aff_count; i++)
		{
			mdl_key_matrix(anim->keys_interp[i], mat_animate_list[track->bone_aff_ids[i]]);
		}
		return True;
	}
	return False;
}

// 跳转帧
//! 如果是逆向播放, 需要设置 time_curr = time_total
Bool mdl_anim_goto(mdl_animate_t *anim, float target_time, float goto_time)
{
	if(anim && anim->track)
	{
		mdl_track_t *track = anim->track;

		// 得到当前插值帧键
		mdl_anim_interp(anim);

		// 复制到跳转起始
		memmove(anim->goto_proc.start_keys, anim->keys_interp, sizeof(mdl_key_t) * track->bone_aff_count);
		anim->goto_proc.start_time = anim->time;

		// 插值得到跳转终止
		mdl_track_interp(track, target_time, anim->target_frms, anim->goto_proc.end_keys);
		anim->goto_proc.end_time = target_time;

		anim->goto_proc.time_total = goto_time;
		anim->goto_proc.time_curr = 0.0;
		anim->goto_proc.weight = 0.0;

		anim->b_goto = True;

		return True;
	}
	return False;
}

// 渐进帧
Bool mdl_anim_advance(mdl_animate_t *anim, float delta)
{
	if(anim && delta != 0.0)
	{
		if(anim->b_goto)
		{
			anim->goto_proc.time_curr += delta;

			if(anim->goto_proc.time_curr > anim->goto_proc.time_total)
			{
				float tmp = anim->goto_proc.time_curr - anim->goto_proc.time_total;
				anim->b_goto = False;
				anim->time = anim->goto_proc.end_time + tmp;
			}
			else if(anim->goto_proc.time_curr < 0.0)
			{
				float tmp = anim->goto_proc.time_curr;
				anim->b_goto = False;
				anim->time = anim->goto_proc.start_time + tmp;
			}
			else
			{
				anim->goto_proc.weight = anim->goto_proc.time_curr / anim->goto_proc.time_total;
			}
		}
		else
		{
			anim->time += delta;
		}
		return True;
	}
	return False;
}

Bool mdl_anim_set_time(mdl_animate_t *anim, float target_time)
{
	if(anim)
	{
		anim->time = target_time;
		return True;
	}
	return False;
}