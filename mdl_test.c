// 101230
// 110107 再次骨骼动画
// 110128 再次骨骼动画
// 110129 第一次实现凹凸贴图+骨骼动画

#include <string.h>
#include "mdl_test.h"
#include "mdlcalc.h"

#include "ut.h"
#include "ms3dconv.h"

vid_program_t *g_vtest_vp, *g_vtest_fp;

vid_param_t *g_vtest_matWVP, *g_vtest_matWorldView, *g_vtest_matBones;
vid_param_t *g_vtest_Position, *g_vtest_TexCoord, *g_vtest_Normal, *g_vtest_Tangent, *g_vtest_Binormal;
vid_param_t *g_vtest_BlendIndex, *g_vtest_BlendWeight;

vid_param_t *g_vtest_samTex, *g_vtest_samTexBump, *g_vtest_samTexSpec;

vid_array_t *g_vtest_vbuf, *g_vtest_nbuf, *g_vtest_tbuf, *g_vtest_bnbuf, *g_vtest_ibuf, *g_vtest_texcbuf;
vid_array_t *g_vtest_bibuf, *g_vtest_bwbuf;

vid_texture_t *g_vtest_tex, *g_vtest_texBump, *g_vtest_texSpec;

//////////////////////////////////////////////////////////////////////////
mdl_skeleton_t *g_skel;
mdl_track_t *g_track;
mdl_animate_t *g_anim;
mdl_mesh_t *g_mesh;

// 矩阵的个数都应该是骨架的所有骨骼数
mat4 *g_mat_anim_list;
mat4 *g_mat_glo_anim;
mat4 *g_mat_loc_anim;

mat4 *g_mat_loc_skel;
mat4 *g_mat_iglo_skel;

//////////////////////////////////////////////////////////////////////////
void mdl_test_init()
{
	{
		uint len;
		ms3d_model_t model;

		void *buf = ut_file_read(L"M_M001.ms3d", &len);
		if(buf)
		{
			if(ms3d_load(buf, len, &model))
			{
				mdl_factory_open();

				// 骨架
				mdl_skel_create(&g_skel);
				ms3d_to_mdl_skel_pre(&model, g_skel);
				ms3d_to_mdl_skel(&model, g_skel);
				// 动画链
				mdl_track_create(&g_track);
				ms3d_to_mdl_track_pre(&model, g_track, 0, 0);
				ms3d_to_mdl_track(&model, g_track);
				// 网格
				mdl_mesh_create(&g_mesh);
				ms3d_to_mdl_mesh_pre(&model, g_mesh);
				ms3d_to_mdl_mesh(&model, g_mesh);

				//! 计算新法线/切线等
				mdl_mesh_calc_tangent(g_mesh, True, True, True);

				// 动画用矩阵列表
				g_mat_loc_anim = (mat4*)ut_alloc(sizeof(mat4) * g_skel->bone_count);
				g_mat_glo_anim = (mat4*)ut_alloc(sizeof(mat4) * g_skel->bone_count);

				g_mat_loc_skel = (mat4*)ut_alloc(sizeof(mat4) * g_skel->bone_count);
				g_mat_iglo_skel = (mat4*)ut_alloc(sizeof(mat4) * g_skel->bone_count);

				g_mat_anim_list = (mat4*)ut_alloc(sizeof(mat4) * g_skel->bone_count);

				// 更新骨架
				mdl_skel_update(g_skel, g_mat_loc_skel, g_mat_iglo_skel);

				// 初始化动画
				mdl_anim_create(&g_anim);
				g_anim->time = 1.0;
				// 绑定动画链
				mdl_anim_bind_track(g_anim, g_track);

				// 释放ms3d
				ms3d_unload(&model);
			}

			ut_file_free(buf);
		}
	}

	// 顶点处理程序
	vid_prog_create(&g_vtest_vp, VID_VERTEX_PROGRAM, True, True, "vert.cg", "vp_main");

	// 像素处理程序
	vid_prog_create(&g_vtest_fp, VID_FRAGMENT_PROGRAM, True, True, "frag.cg", "fp_main");

	// 获得参数
	vid_param_bind(&g_vtest_matWVP, g_vtest_vp, "matWVP");
	vid_param_bind(&g_vtest_matWorldView, g_vtest_vp, "matWorldView");
	vid_param_bind(&g_vtest_matBones, g_vtest_vp, "matBones");

	vid_param_bind(&g_vtest_Position, g_vtest_vp, "Position");
	vid_param_bind(&g_vtest_Normal, g_vtest_vp, "Normal");
	vid_param_bind(&g_vtest_Tangent, g_vtest_vp, "Tangent");
	vid_param_bind(&g_vtest_Binormal, g_vtest_vp, "Binormal");
	vid_param_bind(&g_vtest_TexCoord, g_vtest_vp, "Texcoord");

	vid_param_bind(&g_vtest_BlendIndex, g_vtest_vp, "BlendIndex");
	vid_param_bind(&g_vtest_BlendWeight, g_vtest_vp, "BlendWeight");

	vid_param_bind(&g_vtest_samTex, g_vtest_fp, "baseMap");
	vid_param_bind(&g_vtest_samTexBump, g_vtest_fp, "bumpMap");
	vid_param_bind(&g_vtest_samTexSpec, g_vtest_fp, "specMap");

	// 填写顶点索引
	vid_arr_create(&g_vtest_vbuf, VID_VERTEX_ATTRIB, GL_STATIC_DRAW, sizeof(vec3), g_mesh->vertices, sizeof(vec3) * g_mesh->vert_count);
	vid_arr_create(&g_vtest_nbuf, VID_VERTEX_ATTRIB, GL_STATIC_DRAW, sizeof(vec3), g_mesh->normals, sizeof(vec3) * g_mesh->vert_count);
	vid_arr_create(&g_vtest_tbuf, VID_VERTEX_ATTRIB, GL_STATIC_DRAW, sizeof(vec3), g_mesh->tangents, sizeof(vec3) * g_mesh->vert_count);
	vid_arr_create(&g_vtest_bnbuf, VID_VERTEX_ATTRIB, GL_STATIC_DRAW, sizeof(vec3), g_mesh->binormals, sizeof(vec3) * g_mesh->vert_count);
	{
		ushort4 *bibuf;
		vec4 *bwbuf;
		mdl_mesh_infl_stream(g_mesh, &bibuf, &bwbuf);

		vid_arr_create(&g_vtest_bibuf, VID_VERTEX_ATTRIB, GL_STATIC_DRAW, sizeof(ushort4), bibuf, sizeof(ushort4) * g_mesh->vert_count);
		vid_arr_create(&g_vtest_bwbuf, VID_VERTEX_ATTRIB, GL_STATIC_DRAW, sizeof(vec4), bwbuf, sizeof(vec4) * g_mesh->vert_count);

		mdl_mesh_infl_stream_free(bibuf, bwbuf);

	}

	vid_arr_create(&g_vtest_texcbuf, VID_VERTEX_ATTRIB, GL_STATIC_DRAW, sizeof(vec2), g_mesh->texcoords, sizeof(vec2) * g_mesh->vert_count);

	vid_arr_create(&g_vtest_ibuf, VID_INDEX, GL_STATIC_DRAW, 0, g_mesh->indices, sizeof(ushort) * g_mesh->indi_count);

	// 纹理
	vid_tex_create_dds_file(&g_vtest_tex, L"base.dds", True);
	vid_tex_linear_mipmap(g_vtest_tex);
	vid_tex_wrap_edge(g_vtest_tex);
	vid_tex_anisotropy(g_vtest_tex, 8);

	vid_tex_create_dds_file(&g_vtest_texBump, L"bump.dds", True);
	vid_tex_linear_mipmap(g_vtest_texBump);
	vid_tex_wrap_edge(g_vtest_texBump);
	vid_tex_anisotropy(g_vtest_texBump, 8);

	vid_tex_create_dds_file(&g_vtest_texSpec, L"spec.dds", True);
	vid_tex_linear_mipmap(g_vtest_texSpec);
	vid_tex_wrap_edge(g_vtest_texSpec);
	vid_tex_anisotropy(g_vtest_texSpec, 8);

	// 渲染设置
	// 设置顶点属性
	vid_param_set_vertices(g_vtest_Position, g_vtest_vbuf, GL_FLOAT, 3, 0);
	vid_param_set_vertices(g_vtest_Normal, g_vtest_nbuf, GL_FLOAT, 3, 0);
	vid_param_set_vertices(g_vtest_Tangent, g_vtest_tbuf, GL_FLOAT, 3, 0);
	vid_param_set_vertices(g_vtest_Binormal, g_vtest_bnbuf, GL_FLOAT, 3, 0);
	vid_param_set_vertices(g_vtest_TexCoord, g_vtest_texcbuf, GL_FLOAT, 2, 0);

	vid_param_set_vertices(g_vtest_BlendIndex, g_vtest_bibuf, GL_UNSIGNED_SHORT, 4, 0);
	vid_param_set_vertices(g_vtest_BlendWeight, g_vtest_bwbuf, GL_FLOAT, 4, 0);

	// 开启顶点属性
	vid_param_enable_vertices(g_vtest_Position, True);
	vid_param_enable_vertices(g_vtest_Normal, True);
	vid_param_enable_vertices(g_vtest_Tangent, True);
	vid_param_enable_vertices(g_vtest_Binormal, True);
	vid_param_enable_vertices(g_vtest_TexCoord, True);

	vid_param_enable_vertices(g_vtest_BlendIndex, True);
	vid_param_enable_vertices(g_vtest_BlendWeight, True);

	// 设置索引缓存
	vid_arr_as_indices(g_vtest_ibuf, GL_UNSIGNED_SHORT, 0);
}

void mdl_test_free()
{
	{
		ut_free(g_mat_loc_anim);
		ut_free(g_mat_glo_anim);

		ut_free(g_mat_loc_skel);
		ut_free(g_mat_iglo_skel);

		ut_free(g_mat_anim_list);

		mdl_anim_release(&g_anim);
		mdl_mesh_release(&g_mesh);
		mdl_track_release(&g_track);
		mdl_skel_release(&g_skel);

		mdl_factory_close();
	}

	vid_tex_release(&g_vtest_tex);
	vid_tex_release(&g_vtest_texBump);
	vid_tex_release(&g_vtest_texSpec);
	vid_arr_release(&g_vtest_vbuf);
	vid_arr_release(&g_vtest_nbuf);
	vid_arr_release(&g_vtest_tbuf);
	vid_arr_release(&g_vtest_bnbuf);
	vid_arr_release(&g_vtest_texcbuf);
	vid_arr_release(&g_vtest_ibuf);

	vid_arr_release(&g_vtest_bibuf);
	vid_arr_release(&g_vtest_bwbuf);

	vid_param_release(&g_vtest_matWVP);
	vid_param_release(&g_vtest_matWorldView);
	vid_param_release(&g_vtest_matBones);
	vid_param_release(&g_vtest_Position);
	vid_param_release(&g_vtest_Normal);
	vid_param_release(&g_vtest_Tangent);
	vid_param_release(&g_vtest_Binormal);
	vid_param_release(&g_vtest_TexCoord);
	vid_param_release(&g_vtest_BlendIndex);
	vid_param_release(&g_vtest_BlendWeight);

	vid_param_release(&g_vtest_samTex);
	vid_param_release(&g_vtest_samTexBump);
	vid_param_release(&g_vtest_samTexSpec);

	vid_prog_release(&g_vtest_vp);
	vid_prog_release(&g_vtest_fp);
}


float g_mat_array[16*100];

void mdl_test_draw(double deltaTime, const mat4 matWVP, const mat4 matWorldView)
{
	{
		// 渐进帧
		mdl_anim_advance(g_anim, deltaTime * 24.0f);
		// 初始化插值
		mdl_anim_interp(g_anim);
		// 计算动画矩阵列表
		mdl_anim_matrix(g_anim, g_mat_anim_list);
		// 算出动画后的骨骼矩阵
		mdl_skel_animate(g_skel, g_mat_loc_skel, g_mat_iglo_skel, g_mat_anim_list, g_mat_loc_anim, g_mat_glo_anim);
		// 用矩阵插值顶点
		//mdl_mesh_blend(g_mesh, g_mat_glo_anim, g_vertpool, g_normpool, g_tangpool, g_binopool);

		mdl_skel_final_matrix_array(g_skel, g_mat_glo_anim, g_mat_array, 100);

		vid_param_set_float_array(g_vtest_matBones, g_mat_array, 16*100);
	}

	// 开启处理程序
	vid_prog_enable(g_vtest_vp);
	vid_prog_enable(g_vtest_fp);

	// 设置矩阵
	vid_param_set_matrix(g_vtest_matWVP, matWVP);
	vid_param_set_matrix(g_vtest_matWorldView, matWorldView);

	// 设置纹理
	vid_param_set_texture(g_vtest_samTex, g_vtest_tex);
	// 开启纹理
	vid_param_enable_texture(g_vtest_samTex, True);

	// 设置纹理
	vid_param_set_texture(g_vtest_samTexBump, g_vtest_texBump);
	// 开启纹理
	vid_param_enable_texture(g_vtest_samTexBump, True);

	// 设置纹理
	vid_param_set_texture(g_vtest_samTexSpec, g_vtest_texSpec);
	// 开启纹理
	vid_param_enable_texture(g_vtest_samTexSpec, True);

	// 更新处理程序的变量
	vid_prog_update_params(g_vtest_vp);
	vid_prog_update_params(g_vtest_fp);

	// 调用渲染绘制
	vid_sys_render(GL_TRIANGLES, g_mesh->indi_count);
}

