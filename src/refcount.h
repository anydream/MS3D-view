// 110124

#ifndef __RefCount_h__
#define __RefCount_h__

// 定义引用计数
#define refcount_def			uint ref_count
#define refcount_init(_p)		((_p)->ref_count = 0)
//! 只能用作变量
#define reference(_p)			((_p) ? ((_p)->ref_count += 1, _p) : 0)
#define reference_release(_p)	((_p) ? ((_p)->ref_count -= 1, (_p)->ref_count == 0) : 0)

#endif