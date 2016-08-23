// 110126

#include "mdlcalc.h"
#include "ut.h"
#include "factory.h"
#include <string.h>

extern factory_t g_factory_mdl_animate;

#define mdl_fac_anim_yield			((mdl_animate_t*)factory_yield(&g_factory_mdl_animate))
#define mdl_fac_anim_recycle(_p)	(factory_recycle(&g_factory_mdl_animate, _p))

//////////////////////////////////////////////////////////////////////////
// ��ʼ�������ṹ
Bool mdl_anim_create(mdl_animate_t **anim)
{
	if(anim)
	{
		// �����ṹ, ���Դӹ���������һ���ṹ
		*anim = mdl_fac_anim_yield;

		// ��ʱ������д�ṹ�е�����
		memset(*anim, 0, sizeof(mdl_animate_t));

		// ��ʼ�����ü���Ϊ0
		refcount_init(*anim);

		// ���һ������
		reference(*anim);

		return True;
	}
	return False;
}

// �ͷŶ����ṹ��ָ���Ա��ά�ֵĿռ�
void mdl_anim_release(mdl_animate_t **anim)
{
	if(anim)
	{
		// �������Ϊ0
		if(reference_release(*anim))
		{
			// �ͷų�Ա����
			{
				ut_free((*anim)->goto_proc.start_keys);
				ut_free((*anim)->goto_proc.end_keys);
				ut_free((*anim)->target_frms);
				ut_free((*anim)->keys_interp);

				mdl_track_release(&(*anim)->track);
			}

			// �ͷŽṹʵ��, �����ù�������
			mdl_fac_anim_recycle(*anim);
		}
		*anim = 0;
	}
}

// ��һ��������������, ͬʱ�����ֵ�б�ռ�, �Լ����ڼ���Ѱ֡��Ŀ��֡�±��б�ռ�
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

// ��ֵ��ǰ����
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

// �Ѳ�ֵ���ת���ɾ���, ���滻�����������б�
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

// ��ת֡
//! ��������򲥷�, ��Ҫ���� time_curr = time_total
Bool mdl_anim_goto(mdl_animate_t *anim, float target_time, float goto_time)
{
	if(anim && anim->track)
	{
		mdl_track_t *track = anim->track;

		// �õ���ǰ��ֵ֡��
		mdl_anim_interp(anim);

		// ���Ƶ���ת��ʼ
		memmove(anim->goto_proc.start_keys, anim->keys_interp, sizeof(mdl_key_t) * track->bone_aff_count);
		anim->goto_proc.start_time = anim->time;

		// ��ֵ�õ���ת��ֹ
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

// ����֡
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