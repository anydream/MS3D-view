// 110124

#ifndef __RefCount_h__
#define __RefCount_h__

// �������ü���
#define refcount_def			uint ref_count
#define refcount_init(_p)		((_p)->ref_count = 0)
//! ֻ����������
#define reference(_p)			((_p) ? ((_p)->ref_count += 1, _p) : 0)
#define reference_release(_p)	((_p) ? ((_p)->ref_count -= 1, (_p)->ref_count == 0) : 0)

#endif