// 110126

#include "mdlcalc.h"
#include "ut.h"
#include "factory.h"
#include <string.h>

extern factory_t g_factory_mdl_track;

#define mdl_fac_track_yield			((mdl_track_t*)factory_yield(&g_factory_mdl_track))
#define mdl_fac_track_recycle(_p)	(factory_recycle(&g_factory_mdl_track, _p))

//////////////////////////////////////////////////////////////////////////
// ͨ��ָ����Ŀ��֡�±��ʱ��, ���ʵ�ʵ�ǰ����֡�±��Լ�Ȩ��
// target_frm ֻ����Ϊ���ٵĲ���, ���Ը�ֵΪ 0, ���ӵ�һ֡��ʼ����
Bool mdl_frames_get_id(const mdl_frames_t *frms, float time, uint target_frm,
					   uint *out_curr_id, uint *out_next_id, float *out_weight)
{
	if(frms && target_frm < frms->frm_count)
	{
		uint i;
		uint curr_id = 0, next_id = 0;
		float curr_weight = 0.0;
		Bool flag = False;

		// �Ƚ�ʱ��, �ж������ı߱���
		if(time >= frms->frames[target_frm].time)
		{
			for(i = target_frm; i < frms->frm_count - 1; i++)
			{
				if(time >= frms->frames[i].time && time < frms->frames[i + 1].time)
				{
					curr_id = i;
					next_id = i + 1;
					curr_weight = (time - frms->frames[i].time) / (frms->frames[i + 1].time - frms->frames[i].time);
					flag = True;
					break;
				}
			}
		}
		else
		{
			for(i = target_frm; i > 0; i--)
			{
				if(time > frms->frames[i - 1].time && time <= frms->frames[i].time)
				{
					curr_id = i;
					next_id = i - 1;
					curr_weight = (frms->frames[i].time - time) / (frms->frames[i].time - frms->frames[i - 1].time);
					flag = True;
					break;
				}
			}
		}

		// û�г�����Χ
		if(flag)
		{
			if(out_curr_id) *out_curr_id = curr_id;
			if(out_next_id) *out_next_id = next_id;
			if(out_weight) *out_weight = curr_weight;

			return True;
		}
	}
	return False;
}

// ��ֵ�ؼ�֡
Bool mdl_frames_interp(const mdl_frames_t *frms, float time, uint *target_frm, mdl_key_t *key_out)
{
	uint curr_id, next_id;
	float weight;

	// ���ǰ��֡�±��Ȩ��
	if(key_out && mdl_frames_get_id(frms, time, target_frm ? *target_frm : 0, &curr_id, &next_id, &weight))
	{
		// �����ֵ��
		if(target_frm) *target_frm = curr_id;
		return mdl_key_interp(&frms->frames[curr_id].key, &frms->frames[next_id].key, weight, key_out);
	}
	return False;
}

Bool mdl_track_create(mdl_track_t **track)
{
	if(track)
	{
		// �����ṹ, ���Դӹ���������һ���ṹ
		*track = mdl_fac_track_yield;

		// ��ʱ������д�ṹ�е�����
		memset(*track, 0, sizeof(mdl_track_t));

		// ��ʼ�����ü���Ϊ0
		refcount_init(*track);

		// ���һ������
		reference(*track);

		return True;
	}
	return False;
}

void mdl_track_release(mdl_track_t **track)
{
	if(track)
	{
		// �������Ϊ0
		if(reference_release(*track))
		{
			// �ͷų�Ա����
			{
				ut_free((*track)->bone_aff_ids);
				ut_free((*track)->tracks);
				ut_free((*track)->keyfrm_pool);
			}

			// �ͷŽṹʵ��, �����ù�������
			mdl_fac_track_recycle(*track);
		}
		*track = 0;
	}
}

// ��ֵ������
Bool mdl_track_interp(const mdl_track_t *track, float time, uint *target_frms, mdl_key_t *keys_out)
{
	if(track && keys_out)
	{
		ushort i;
		Bool res = True;

		for(i = 0; i < track->bone_aff_count; i++)
		{
			res = mdl_frames_interp(&track->tracks[i], time, target_frms ? &target_frms[i] : 0, &keys_out[i]) && res;
		}
		return res;
	}
	return False;
}
