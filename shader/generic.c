// 110202

#include "../src/vid.h" //! ������Ҫһ�� include �ļ���
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
		// �����ṹ, ���Դӹ���������һ���ṹ
		*vms = vmdl_fac_mesh_stream_yield;

		// ��ʱ������д�ṹ�е�����
		memset(*vms, 0, sizeof(vmdl_mesh_stream_t));

		// ��ʼ�����ü���Ϊ0
		refcount_init(*vms);

		// ���һ������
		reference(*vms);

		return True;
	}
	return False;
}

void vmdl_mesh_stream_release(vmdl_mesh_stream_t **vms)
{
	if(vms)
	{
		// �������Ϊ0
		if(reference_release(*vms))
		{
			// �ͷų�Ա����
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

			// �ͷŽṹʵ��, �����ù�������
			vmdl_fac_mesh_stream_recycle(*vms);
		}
		*vms = 0;
	}
}

void vmdl_mesh_stream_attach(vmdl_mesh_stream_t *vms, const mdl_mesh_t *mesh)
{

}*/