// 110202

#include "../src/vid.h" //! 或许需要一个 include 文件夹
#include "../src/vid.h"

typedef struct _vmdl_mesh_stream
{
	vid_array_t	*vert_stream;
	vid_array_t *norm_stream;
	vid_array_t *tangent_stream;
	vid_array_t *binorm_stream;
	vid_array_t *bindex_stream;
	vid_array_t *bweight_stream;
	vid_array_t *texc_stream;
	vid_array_t *indices_stream;
} vmdl_mesh_stream_t;

/*
Bool vmdl_mesh_stream_create(vmdl_mesh_stream_t **vms)
{
	if(vms)
	{
		// 创建结构, 可以从工厂中生产一个结构
		*vms = vmdl_fac_mesh_stream_yield;

		// 此时可以填写结构中的数据
		memset(*vms, 0, sizeof(vmdl_mesh_stream_t));

		// 初始化引用计数为0
		refcount_init(*vms);

		// 添加一次引用
		reference(*vms);

		return True;
	}
	return False;
}

void vmdl_mesh_stream_release(vmdl_mesh_stream_t **vms)
{
	if(vms)
	{
		// 如果引用为0
		if(reference_release(*vms))
		{
			// 释放成员数据
			{
				vid_arr_release((*vms)->vert_stream);
				vid_arr_release((*vms)->norm_stream);
				vid_arr_release((*vms)->tangent_stream);
				vid_arr_release((*vms)->binorm_stream);
				vid_arr_release((*vms)->bindex_stream);
				vid_arr_release((*vms)->bweight_stream);
				vid_arr_release((*vms)->texc_stream);
				vid_arr_release((*vms)->indices_stream);
			}

			// 释放结构实体, 可以让工厂回收
			vmdl_fac_mesh_stream_recycle(*vms);
		}
		*vms = 0;
	}
}

void vmdl_mesh_stream_attach(vmdl_mesh_stream_t *vms, const mdl_mesh_t *mesh)
{

}*/