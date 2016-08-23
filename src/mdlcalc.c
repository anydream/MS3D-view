
#include "mdlcalc.h"
#include "factory.h"

//////////////////////////////////////////////////////////////////////////
// mdl 渲染结构的工厂
factory_t g_factory_mdl_mesh;
factory_t g_factory_mdl_skeleton;
factory_t g_factory_mdl_track;
factory_t g_factory_mdl_animate;
factory_t g_factory_mdl_node;

//////////////////////////////////////////////////////////////////////////
Bool mdl_factory_open()
{
	Bool res;
	res = factory_open(&g_factory_mdl_mesh,		sizeof(mdl_mesh_t),		64, 64);
	res = factory_open(&g_factory_mdl_skeleton,	sizeof(mdl_skeleton_t),	64, 64) && res;
	res = factory_open(&g_factory_mdl_track,	sizeof(mdl_track_t),	64, 64) && res;
	res = factory_open(&g_factory_mdl_animate,	sizeof(mdl_animate_t),	64, 64) && res;
	res = factory_open(&g_factory_mdl_node,		sizeof(mdl_node_t),		64, 64) && res;

	return res;
}

Bool mdl_factory_close()
{
	Bool res;
	res = factory_close(&g_factory_mdl_mesh);
	res = factory_close(&g_factory_mdl_skeleton) && res;
	res = factory_close(&g_factory_mdl_track) && res;
	res = factory_close(&g_factory_mdl_animate) && res;
	res = factory_close(&g_factory_mdl_node) && res;

	return res;
}
