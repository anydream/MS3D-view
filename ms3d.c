// 110106 检查

#include "ms3d.h"
#include "ut.h"
#include <string.h>

Bool ms3d_read_buf(void *buf, uint buflen,
				   ms3d_vertex_t vertices[],
				   ms3d_triangle_t triangles[],
				   ms3d_group_t groups[], ushort triangle_id[],
				   ms3d_material_t materials[],
				   ms3d_joint_t joints[], ms3d_frame_t rotates[], ms3d_frame_t translates[],
				   ushort *pVerticesCnt, ushort *pTrianglesCnt, ushort *pGroupsCnt, ushort *pMtrlCnt,
				   ushort *pJointsCnt, uint *pRotatesCnt, uint *pTranslatesCnt,
				   float *fps, int *total_frms)
{
	if(buf)
	{
		uchar *offset = (uchar*)buf;
		uint i, j;
		int version;
		ushort numVertices, numTriangles, numGroups, numMaterials, numJoints;
		uint numTriangleIDs = 0, numRotates = 0, numTranslates = 0;
		
		float animFPS, currTime;
		int totalFrames;
		
		// 比较魔数
		if(memcmp(offset, "MS3D000000", 10))
			return False;
		offset += 10;
		
		// 文件版本
		version = AS_INT(offset);
		if(version != 4) return False;
		
		// 顶点个数
		numVertices = AS_USHORT(offset);
		
		// 读取顶点
		if(vertices)
		{
			for(i = 0; i < numVertices; i++)
			{
				offset++;
				READ_FLOAT3(vertices[i].vertex_pos, offset);
				vertices[i].bone_id = AS_CHAR(offset);
				offset++;
			}
		}
		else
		{
			offset += numVertices * MS3D_VERTEX_SIZE;
		}
		
		// 三角形个数
		numTriangles = AS_USHORT(offset);
		
		// 读取三角形
		if(triangles)
		{
			for(i = 0; i < numTriangles; i++)
			{
				offset += sizeof(ushort);
				READ_USHORT3(triangles[i].vertex_id, offset);
				
				for(j = 0; j < 9; j++)
					triangles[i].vertex_normal[j] = AS_FLOAT(offset);
				
				READ_FLOAT3(triangles[i].s, offset);
				READ_FLOAT3(triangles[i].t, offset);
				
				triangles[i].smooth_group = AS_UCHAR(offset);
				triangles[i].group_id = AS_UCHAR(offset);
			}
		}
		else
		{
			offset += numTriangles * MS3D_TRIANGLE_SIZE;
		}
		
		// 组个数
		numGroups = AS_USHORT(offset);
		
		// 读取组
		for(i = 0; i < numGroups; i++)
		{
			ushort group_triangles;
			if(groups)
			{
				offset++;
				memmove(groups[i].name, offset, MS3D_MAX_NAME_SIZE);
				offset += MS3D_MAX_NAME_SIZE;
			}
			else offset += MS3D_MAX_NAME_SIZE + sizeof(uchar);
			
			group_triangles = AS_USHORT(offset);
			
			if(groups) groups[i].group_triangles = group_triangles;
			
			if(triangle_id)
			{
				for(j = 0; j < group_triangles; j++)
					triangle_id[numTriangleIDs + j] = AS_USHORT(offset);
			}
			else offset += sizeof(ushort) * group_triangles;
			
			numTriangleIDs += group_triangles;
			
			if(groups) groups[i].material_id = AS_CHAR(offset);
			else offset += sizeof(char);
		}
		
		// 材质个数
		numMaterials = AS_USHORT(offset);
		
		// 读取材质
		if(materials)
		{
			for(i = 0; i < numMaterials; i++)
			{
				memmove(materials[i].name, offset, MS3D_MAX_NAME_SIZE);
				offset += MS3D_MAX_NAME_SIZE;
				
				READ_FLOAT4(materials[i].ambient, offset);
				READ_FLOAT4(materials[i].diffuse, offset);
				READ_FLOAT4(materials[i].specular, offset);
				READ_FLOAT4(materials[i].emissive, offset);
				materials[i].shininess = AS_FLOAT(offset);
				materials[i].transparency = AS_FLOAT(offset);
				materials[i].mode = AS_UCHAR(offset);
				
				memmove(materials[i].diffuse_texture, offset, MS3D_MAX_TEXTURE_FILENAME_SIZE);
				offset += MS3D_MAX_TEXTURE_FILENAME_SIZE;
				memmove(materials[i].alpha_texture, offset, MS3D_MAX_TEXTURE_FILENAME_SIZE);
				offset += MS3D_MAX_TEXTURE_FILENAME_SIZE;
				
				materials[i].ambient[3] = materials[i].transparency;
				materials[i].diffuse[3] = materials[i].transparency;
				materials[i].specular[3] = materials[i].transparency;
				materials[i].emissive[3] = materials[i].transparency;
			}
		}
		else
		{
			offset += numMaterials * MS3D_MATERIAL_SIZE;
		}
		
		// 帧率
		animFPS = AS_FLOAT(offset);
		if(animFPS < 1.0) animFPS = 1.0;
		// 时间
		currTime = AS_FLOAT(offset);
		// 总帧数
		totalFrames = AS_INT(offset);
		
		// 节点
		numJoints = AS_USHORT(offset);
		
		// 读取节点
		for(i = 0; i < numJoints; i++)
		{
			ushort rot_count, pos_count;
			if(joints)
			{
				offset++;
				memmove(joints[i].name, offset, MS3D_MAX_NAME_SIZE);
				offset += MS3D_MAX_NAME_SIZE;
				memmove(joints[i].parent_name, offset, MS3D_MAX_NAME_SIZE);
				offset += MS3D_MAX_NAME_SIZE;
				READ_FLOAT3(joints[i].rot, offset);
				READ_FLOAT3(joints[i].pos, offset);
			}
			else offset += MS3D_JOINT_SIZE - sizeof(ushort) * 2;
			
			rot_count = AS_USHORT(offset);
			pos_count = AS_USHORT(offset);
			
			if(joints)
			{
				joints[i].rot_count = rot_count;
				joints[i].pos_count = pos_count;
			}
			
			// 旋转帧
			if(rotates)
			{
				for(j = 0; j < rot_count; j++)
				{
					rotates[numRotates + j].time = AS_FLOAT(offset);
					READ_FLOAT3(rotates[numRotates + j].key, offset);
					rotates[numRotates + j].time *= animFPS;
				}
			}
			else offset += rot_count * MS3D_FRAME_SIZE;
			
			// 位移帧
			if(translates)
			{
				for(j = 0; j < pos_count; j++)
				{
					translates[numTranslates + j].time = AS_FLOAT(offset);
					READ_FLOAT3(translates[numTranslates + j].key, offset);
					translates[numTranslates + j].time *= animFPS;
				}
			}
			else offset += pos_count * MS3D_FRAME_SIZE;
			
			numRotates += rot_count;
			numTranslates += pos_count;
		}
		
		// 跳过注释
		if(offset < (uchar*)buf + buflen)
		{
			int subVersion = AS_INT(offset);
			if(subVersion == 1)
			{
				int numComments;
				int commSize;
				int k;
				for(j = 0; j < 3; j++)
				{
					numComments = AS_INT(offset);
					for(k = 0; k < numComments; k++)
					{
						offset += sizeof(int);
						commSize = AS_INT(offset);
						offset += commSize;
					}
				}
				
				numComments = AS_INT(offset);
				if(numComments == 1)
				{
					commSize = AS_INT(offset);
					offset += commSize;
				}
			}
			else return False;
		}
		
		// 顶点扩展数据
		if(offset < (uchar*)buf + buflen)
		{
			int subVersion = AS_INT(offset);
			// 110201 19:12 添加, 用于读取附加的顶点索引
			if(subVersion == 3)
			{
				if(vertices)
				{
					for(i = 0; i < numVertices; i++)
					{
						READ_CHAR3(vertices[i].bone_ids, offset);
						READ_UCHAR3(vertices[i].weight, offset);
						offset += sizeof(uint) * 2;
					}
				}
				else offset += numVertices * (MS3D_VERTEX_EXTRA_SIZE + sizeof(uint) * 2);
			}
			else if(subVersion == 2)
			{
				if(vertices)
				{
					for(i = 0; i < numVertices; i++)
					{
						READ_CHAR3(vertices[i].bone_ids, offset);
						READ_UCHAR3(vertices[i].weight, offset);
						offset += sizeof(uint);
					}
				}
				else offset += numVertices * (MS3D_VERTEX_EXTRA_SIZE + sizeof(uint));
			}
			else if(subVersion == 1)
			{
				if(vertices)
				{
					for(i = 0; i < numVertices; i++)
					{
						READ_CHAR3(vertices[i].bone_ids, offset);
						READ_UCHAR3(vertices[i].weight, offset);
					}
				}
				else offset += numVertices * MS3D_VERTEX_EXTRA_SIZE;
			}
		}
		
		// 其余的数据忽略

		// (110106) 如果不相等, 则说明有问题
		if(numTriangles != numTriangleIDs) return False;
		
		if(pVerticesCnt) *pVerticesCnt = numVertices;
		if(pTrianglesCnt) *pTrianglesCnt = numTriangles;
		if(pGroupsCnt) *pGroupsCnt = numGroups;
		if(pMtrlCnt) *pMtrlCnt = numMaterials;
		if(pJointsCnt) *pJointsCnt = numJoints;
		if(pRotatesCnt) *pRotatesCnt = numRotates;
		if(pTranslatesCnt) *pTranslatesCnt = numTranslates;
		if(fps) *fps = animFPS;
		if(total_frms) *total_frms = totalFrames;
		
		return True;
	}
	return False;
}

Bool ms3d_load(void *buf, uint buflen, ms3d_model_t *model)
{
	if(buf && buflen && model)
	{
		ushort numVerts, numTris, numGroups, numMtrls, numJoints;
		uint numRotates, numTranslates;
		float fps;
		int totalFrms;
		
		if(ms3d_read_buf(buf, buflen, 0, 0, 0, 0, 0, 0, 0, 0,
			&numVerts, &numTris, &numGroups,
			&numMtrls, &numJoints, &numRotates, &numTranslates,
			&fps, &totalFrms))
		{
			uint i;
			uint offset, offset2;
			ms3d_vertex_t *vertices = (ms3d_vertex_t*)ut_alloc(sizeof(ms3d_vertex_t) * numVerts);
			ms3d_triangle_t *tris = (ms3d_triangle_t*)ut_alloc(sizeof(ms3d_triangle_t) * numTris);
			ms3d_group_t *groups = (ms3d_group_t*)ut_alloc(sizeof(ms3d_group_t) * numGroups);
			ushort *tri_ids = (ushort*)ut_alloc(sizeof(ushort) * numTris);
			ms3d_material_t *mtrls = (ms3d_material_t*)ut_alloc(sizeof(ms3d_material_t) * numMtrls);
			ms3d_joint_t *joints = (ms3d_joint_t*)ut_alloc(sizeof(ms3d_joint_t) * numJoints);
			ms3d_frame_t *rotates = (ms3d_frame_t*)ut_alloc(sizeof(ms3d_frame_t) * numRotates);
			ms3d_frame_t *translates = (ms3d_frame_t*)ut_alloc(sizeof(ms3d_frame_t) * numTranslates);

			//memset(vertices, 0, sizeof(ms3d_vertex_t) * numVerts);
			for(i = 0; i < numVerts; i++)
			{
				vertices[i].bone_ids[0] = -1;
				vertices[i].bone_ids[1] = -1;
				vertices[i].bone_ids[2] = -1;
				vertices[i].weight[0] = 0;
				vertices[i].weight[1] = 0;
				vertices[i].weight[2] = 0;
			}
			
			ms3d_read_buf(buf, buflen, vertices, tris, groups, tri_ids, mtrls, joints, rotates, translates,
				0, 0, 0, 0, 0, 0, 0, 0, 0);
			
			offset = 0;
			for(i = 0; i < numGroups; i++)
			{
				groups[i].tri_id = tri_ids + offset;
				offset += groups[i].group_triangles;
			}
			offset = 0;
			offset2 = 0;
			for(i = 0; i < numJoints; i++)
			{
				joints[i].rotates = rotates + offset;
				offset += joints[i].rot_count;
				joints[i].translates = translates + offset2;
				offset2 += joints[i].pos_count;
			}

			model->vertices = vertices;
			model->tris = tris;
			model->groups = groups;
			model->tri_ids = tri_ids;
			model->mtrls = mtrls;
			model->joints = joints;
			model->rotates = rotates;
			model->translates = translates;

			model->numVerts = numVerts;
			model->numTris = numTris;
			model->numGroups = numGroups;
			model->numMtrls = numMtrls;
			model->numJoints = numJoints;
			model->numRotates = numRotates;
			model->numTranslates = numTranslates;
			model->fps = fps;
			model->totalFrms = totalFrms;

			return True;
		}
	}
	return False;
}

Bool ms3d_unload(ms3d_model_t *model)
{
	if(model)
	{
		ut_free(model->vertices);
		ut_free(model->tris);
		ut_free(model->groups);
		ut_free(model->tri_ids);
		ut_free(model->mtrls);
		ut_free(model->joints);
		ut_free(model->rotates);
		ut_free(model->translates);
		return True;
	}
	return False;
}

