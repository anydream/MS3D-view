// 101003 ����
// 110104 �޸�

#include "vid.h"
#include "viddebug.h"
#include "dds.h"
#include "ut.h"
#include "factory.h"
#include <string.h>

// ��������
void create_texture(GLuint id, GLenum target, GLuint level, GLint ifmt,
					const GLvoid *pix, uint pixSize,
					GLenum fmt, GLenum type, Bool compressed,
					uint width, uint height, uint depth, Bool autoMipmap)
{
	GLenum tarTot = target;

	// ��������������ĳ����
	if(target >= GL_TEXTURE_CUBE_MAP_POSITIVE_X && target <= GL_TEXTURE_CUBE_MAP_NEGATIVE_Z)
		tarTot = GL_TEXTURE_CUBE_MAP;

	if(id)	// �����Ҫ��
	{
		// �� ID ��Ŀ������
		glBindTexture(tarTot, id);
	}

	if(autoMipmap) glTexParameteri(tarTot, GL_GENERATE_MIPMAP, GL_TRUE);
	else glTexParameteri(tarTot, GL_GENERATE_MIPMAP, GL_FALSE);

	// Ĭ���������Բ����Ա�������ʾ����
	glTexParameteri(tarTot, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(tarTot, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// ��ά���������������
	if(target == GL_TEXTURE_2D || tarTot == GL_TEXTURE_CUBE_MAP)
	{
		if(compressed) glCompressedTexImage2D(target, level, ifmt, width, height, 0, pixSize, pix);
		else glTexImage2D(target, level, ifmt, width, height, 0, fmt, type, pix);
	}
	// ��ά����
	else if(target == GL_TEXTURE_3D)
	{
		if(compressed) glCompressedTexImage3D(GL_TEXTURE_3D, level, ifmt, width, height, depth, 0, pixSize, pix);
		else glTexImage3D(GL_TEXTURE_3D, level, ifmt, width, height, depth, 0, fmt, type, pix);
	}
}

// �޸�����
void modify_texture(GLuint id, GLenum target, GLuint level,
					int xoff, int yoff, int zoff, int width, int height, int depth,
					const GLvoid *pix, uint pixSize,
					GLenum fmt, GLenum type, Bool compressed)
{
	GLenum tarTot = target;

	// ��������������ĳ����
	if(target >= GL_TEXTURE_CUBE_MAP_POSITIVE_X && target <= GL_TEXTURE_CUBE_MAP_NEGATIVE_Z)
		tarTot = GL_TEXTURE_CUBE_MAP;

	if(id)	// �����Ҫ��
	{
		// �� ID ��Ŀ������
		glBindTexture(tarTot, id);
	}

	// ��ά���������������
	if(target == GL_TEXTURE_2D || tarTot == GL_TEXTURE_CUBE_MAP)
	{
		if(compressed) glCompressedTexSubImage2D(target, level, xoff, yoff, width, height, fmt, pixSize, pix);
		else glTexSubImage2D(target, level, xoff, yoff, width, height, fmt, type, pix);
	}
	// ��ά����
	else if(target == GL_TEXTURE_3D)
	{
		if(compressed) glCompressedTexSubImage3D(GL_TEXTURE_3D, level, xoff, yoff, zoff, width, height, depth, fmt, pixSize, pix);
		else glTexSubImage3D(GL_TEXTURE_3D, level, xoff, yoff, zoff, width, height, depth, fmt, type, pix);
	}
}

//////////////////////////////////////////////////////////////////////////
void swap_buffer(void *byte1, void *byte2, void *tmp, int size)
{
	memmove(tmp, byte1, size);
	memmove(byte1, byte2, size);
	memmove(byte2, tmp, size);
}

void swap_uchar(uchar *byte1, uchar *byte2)
{
	uchar tmp = *byte1;
	*byte1 = *byte2;
	*byte2 = tmp;
}

void swap_ushort(ushort *byte1, ushort *byte2)
{
	ushort tmp = *byte1;
	*byte1 = *byte2;
	*byte2 = tmp;
}

void flip_blocks_dxtc1(DXTColBlock *line, int numBlocks)
{
	DXTColBlock *curblock = line;
	int i;

	for(i = 0; i < numBlocks; i++)
	{
		swap_uchar(&curblock->row[0], &curblock->row[3]);
		swap_uchar(&curblock->row[1], &curblock->row[2]);

		curblock++;
	}
}

void flip_blocks_dxtc3(DXTColBlock *line, int numBlocks)
{
	DXTColBlock *curblock = line;
	DXT3AlphaBlock *alphablock;
	int i;

	for(i = 0; i < numBlocks; i++)
	{
		alphablock = (DXT3AlphaBlock*)curblock;

		swap_ushort(&alphablock->row[0], &alphablock->row[3]);
		swap_ushort(&alphablock->row[1], &alphablock->row[2]);

		curblock++;

		swap_uchar(&curblock->row[0], &curblock->row[3]);
		swap_uchar(&curblock->row[1], &curblock->row[2]);

		curblock++;
	}
}

void flip_dxt5_alpha(DXT5AlphaBlock *block)
{
	uchar gBits[4][4];
	uint *pBits;

	const uint mask = 0x00000007;		    // bits = 00 00 01 11
	uint bits = 0;
	memmove(&bits, &block->row[0], sizeof(uchar) * 3);

	gBits[0][0] = (uchar)(bits & mask);
	bits >>= 3;
	gBits[0][1] = (uchar)(bits & mask);
	bits >>= 3;
	gBits[0][2] = (uchar)(bits & mask);
	bits >>= 3;
	gBits[0][3] = (uchar)(bits & mask);
	bits >>= 3;
	gBits[1][0] = (uchar)(bits & mask);
	bits >>= 3;
	gBits[1][1] = (uchar)(bits & mask);
	bits >>= 3;
	gBits[1][2] = (uchar)(bits & mask);
	bits >>= 3;
	gBits[1][3] = (uchar)(bits & mask);

	bits = 0;
	memmove(&bits, &block->row[3], sizeof(uchar) * 3);

	gBits[2][0] = (uchar)(bits & mask);
	bits >>= 3;
	gBits[2][1] = (uchar)(bits & mask);
	bits >>= 3;
	gBits[2][2] = (uchar)(bits & mask);
	bits >>= 3;
	gBits[2][3] = (uchar)(bits & mask);
	bits >>= 3;
	gBits[3][0] = (uchar)(bits & mask);
	bits >>= 3;
	gBits[3][1] = (uchar)(bits & mask);
	bits >>= 3;
	gBits[3][2] = (uchar)(bits & mask);
	bits >>= 3;
	gBits[3][3] = (uchar)(bits & mask);

	pBits = ((uint*)&(block->row[0]));

	*pBits &= 0xff000000;

	*pBits = *pBits | (gBits[3][0] << 0);
	*pBits = *pBits | (gBits[3][1] << 3);
	*pBits = *pBits | (gBits[3][2] << 6);
	*pBits = *pBits | (gBits[3][3] << 9);

	*pBits = *pBits | (gBits[2][0] << 12);
	*pBits = *pBits | (gBits[2][1] << 15);
	*pBits = *pBits | (gBits[2][2] << 18);
	*pBits = *pBits | (gBits[2][3] << 21);

	pBits = ((uint*)&(block->row[3]));

	*pBits &= 0xff000000;

	*pBits = *pBits | (gBits[1][0] << 0);
	*pBits = *pBits | (gBits[1][1] << 3);
	*pBits = *pBits | (gBits[1][2] << 6);
	*pBits = *pBits | (gBits[1][3] << 9);

	*pBits = *pBits | (gBits[0][0] << 12);
	*pBits = *pBits | (gBits[0][1] << 15);
	*pBits = *pBits | (gBits[0][2] << 18);
	*pBits = *pBits | (gBits[0][3] << 21);
}

void flip_blocks_dxtc5(DXTColBlock *line, int numBlocks)
{
	DXTColBlock *curblock = line;
	DXT5AlphaBlock *alphablock;
	int i;

	for(i = 0; i < numBlocks; i++)
	{
		alphablock = (DXT5AlphaBlock*)curblock;

		flip_dxt5_alpha(alphablock);

		curblock++;

		swap_uchar(&curblock->row[0], &curblock->row[3]);
		swap_uchar(&curblock->row[1], &curblock->row[2]);

		curblock++;
	}
}

void flip_dds(uchar *image, Bool compressed, int format, int width, int height, int depth, int size)
{
	int linesize;
	int offset;

	if(!compressed)
	{
		int n, i;
		int imagesize = size / depth;
		void *buf;
		linesize = imagesize / height;
		buf = ut_alloc(linesize);

		for(n = 0; n < depth; n++)
		{
			uchar *top, *bottom;
			offset = imagesize*n;
			top = image + offset;
			bottom = top + (imagesize - linesize);

			for(i = 0; i < (height >> 1); i++)
			{
				swap_buffer(bottom, top, buf, linesize);

				top += linesize;
				bottom -= linesize;
			}
		}
		ut_free(buf);
	}
	else
	{
		int xblocks = width >> 2;
		int yblocks = height >> 2;
		int blocksize;
		int j;

		DXTColBlock *top;
		DXTColBlock *bottom;
		void *buf;

		void (*flipblocks)(DXTColBlock*, int);

		switch(format)
		{
		case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
			blocksize = 8;
			flipblocks = flip_blocks_dxtc1;
			break;
		case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:
			blocksize = 16;
			flipblocks = flip_blocks_dxtc3;
			break;
		case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
			blocksize = 16;
			flipblocks = flip_blocks_dxtc5;
			break;
		default:
			return;
		}

		linesize = xblocks * blocksize;
		buf = ut_alloc(linesize);

		for(j = 0; j < (yblocks >> 1); j++)
		{
			top = (DXTColBlock*)(image + j * linesize);
			bottom = (DXTColBlock*)(image + (((yblocks - j) - 1) * linesize));

			flipblocks(top, xblocks);
			flipblocks(bottom, xblocks);

			swap_buffer(bottom, top, buf, linesize);
		}

		ut_free(buf);
	}
}

Bool create_texture_dds(GLuint id, const void *stream, uint length, Bool autoMipmap, DDSDESC *ddsdesc)
{
	if(id && stream && length)
	{
		DDS_HEADER *header;
		uchar *image;
		uint levels, pflags, bpp, width, height, depth;
		GLint iformat;
		GLenum target, format = 0, type = 0;
		uint components;
		GLboolean volume = False, cubemap = False;
		GLboolean compressed = False;
		uint i, n;

		header = (DDS_HEADER*)stream;

		// �Ϸ��Լ��
		if(header->magic != FOURCC("DDS ")) return False;
		else if(header->size != 124) return False;

		// ָ��ͼ��������ʼλ��
		image = (uchar*)&header[1];

		// ��ȡͼ����Ϣ
		levels = header->mipMapCount;
		if(levels < 1) levels = 1;
		pflags = header->pixelFormat.flags;
		bpp = header->pixelFormat.bitsPerPixel;
		width = header->width;
		height = header->height;
		depth = header->depth;
		if(depth < 1) depth = 1;

		// �Ƿ�Ϊ��ά���� / ��������
		if(header->caps.caps2 & DDS_VOLUME)
		{
			volume = True;
			target = GL_TEXTURE_3D;
			if(ddsdesc) ddsdesc->target = GL_TEXTURE_3D;
		}
		else if(header->caps.caps2 & DDS_CUBEMAP)
		{
			cubemap = True;
			target = GL_TEXTURE_CUBE_MAP_POSITIVE_X;
			if(ddsdesc) ddsdesc->target = GL_TEXTURE_CUBE_MAP;
		}
		else
		{
			target = GL_TEXTURE_2D;
			if(ddsdesc) ddsdesc->target = GL_TEXTURE_2D;
		}

		// �����ʽ
		if(pflags & DDS_FOURCC)
		{
			switch(header->pixelFormat.fourCC)
			{
				// ѹ����ʽ
			case FOURCC_DXT1:
				iformat = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
				compressed = True;
				components = 8;
				break;

			case FOURCC_DXT3:
				iformat = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
				compressed = True;
				components = 16;
				break;

			case FOURCC_DXT5:
				iformat = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
				compressed = True;
				components = 16;
				break;

				// �����ʽ
			case FOURCC_ABGR16F:
				iformat = GL_RGBA16F;
				format = GL_RGBA;
				type = GL_HALF_FLOAT;
				compressed = False;
				components = 8;
				break;

			case FOURCC_ABGR32F:
				iformat = GL_RGBA32F;
				format = GL_RGBA;
				type = GL_FLOAT;
				compressed = False;
				components = 16;
				break;

			default:
				// ��֧�ֵ������ʽ
				return False;
			}
		}
		// R8G8B8A8
		else if(pflags == DDS_RGBA && bpp == 32)
		{
			iformat = GL_RGBA8;
			format = GL_BGRA;
			type = GL_UNSIGNED_BYTE;
			compressed = False;
			components = 4;
		}
		// R8G8B8
		else if(pflags == DDS_RGB && bpp == 24)
		{
			iformat = GL_RGB8;
			format = GL_BGR;
			type = GL_UNSIGNED_BYTE;
			compressed = False;
			components = 3;
		}
		// 8 λ�Ҷ�ͼ
		else if((pflags == DDS_ALPHA || pflags == DDS_LUM) && bpp == 8)
		{
			iformat = GL_LUMINANCE8;	//! ������Ҫ�ĳ�ALPHA
			format = GL_LUMINANCE;
			type = GL_UNSIGNED_BYTE;
			compressed = False;
			components = 1;
		}
		// ������֧�ֵĸ�ʽ
		else return False;

		// ѭ�����������ÿ����
		for(n = 0; n < (cubemap ? 6U : 1U); n++)
		{
			uint w = width;
			uint h = height;
			uint d = depth;

			// ѭ��ÿ�� Mipmap �㼶
			for(i = 0; i < levels; i++)
			{
				// ͼ�����ݴ�С
				uint imageSizeInBytes;
				// ��ȡƫ��
				//uint offsetInToRead;

				// ���㵱ǰͼ�����ݴ�С
				if(compressed) imageSizeInBytes = ((w + 3) >> 2) * ((h + 3) >> 2) * components;
				else imageSizeInBytes = w * h * components;
				// ��ά������Ҫ�������
				if(volume) imageSizeInBytes *= d;

				//! offsetInToRead = (uint)image + imageSizeInBytes - (uint)stream;
				// ������Χ, ��Ȼ���ݲ���ȷ
				//! if(offsetInToRead > length) return False;

				if(!cubemap) flip_dds(image, compressed, iformat, w, h, d, imageSizeInBytes);
				// ��������
				create_texture(id, target, i, iformat, image, imageSizeInBytes, format, type, compressed, w, h, d, autoMipmap);
				if(id) id = 0;

				// ָ����һ��������ʼ
				image += imageSizeInBytes;

				// Mipmap �ߴ����
				w = w >> 1;
				if(w < 1) w = 1;

				h = h >> 1;
				if(h < 1) h = 1;

				if(volume)
				{
					d = d >> 1;
					if(d < 1) d = 1;
				}
			}

			// �������������, ָ����һ����
			if(cubemap) target += 1;
		}

		if(ddsdesc)
		{
			ddsdesc->iformat = iformat;

			ddsdesc->levels = levels;
			ddsdesc->width = width;
			ddsdesc->height = height;
			ddsdesc->depth = depth;

			ddsdesc->compressed = compressed;
			ddsdesc->components = components;
			ddsdesc->bpp = bpp;
			ddsdesc->format = format;
			ddsdesc->type = type;
		}

		return True;
	}
	return False;
}

//////////////////////////////////////////////////////////////////////////
extern factory_t g_factory_vid_texture;

#define vid_fac_tex_yield			((vid_texture_t*)factory_yield(&g_factory_vid_texture))
#define vid_fac_tex_recycle(_p)		(factory_recycle(&g_factory_vid_texture, _p))

Bool vid_tex_create_2D(vid_texture_t **tex,
					   uint levels, GLint ifmt,
					   const void *pix, uint pixSize,
					   GLenum fmt, GLenum type, Bool compressed,
					   uint width, uint height, Bool autoMipmap)
{
	if(width && height)
	{
		GLuint id;
		vid_texture_t temp;

		glGenTextures(1, &id);
		if(!id) RETURN_FALSE;

		create_texture(id, GL_TEXTURE_2D, levels, ifmt, pix, pixSize, fmt, type, compressed, width, height, 0, autoMipmap);

		temp.target = GL_TEXTURE_2D;
		temp.tex_id = id;

		temp.levels = levels;
		temp.width = width;
		temp.height = height;
		temp.depth = 0;

		temp.compressed = compressed;
		temp.iformat = ifmt;
		temp.format = fmt;
		temp.type = type;

		CHECK_GL_ERR;

		// �����ṹ, ���Դӹ���������һ���ṹ
		*tex = vid_fac_tex_yield;

		memmove(*tex, &temp, sizeof(vid_texture_t));

		// ��ʼ�����ü���Ϊ0
		refcount_init(*tex);

		// ���һ������
		reference(*tex);

		RETURN_TRUE;
	}
	RETURN_FALSE;
}

Bool vid_tex_create_dds(vid_texture_t **tex, const void *ptr, uint size, Bool autoMipmap)
{
	if(ptr && size)
	{
		DDSDESC ddsdesc;
		GLuint id;
		vid_texture_t temp;

		glGenTextures(1, &id);
		if(!id) RETURN_FALSE;

		if(!create_texture_dds(id, ptr, size, autoMipmap, &ddsdesc)) RETURN_FALSE;

		temp.target = ddsdesc.target;
		temp.tex_id = id;

		temp.levels = ddsdesc.levels;
		temp.width = ddsdesc.width;
		temp.height = ddsdesc.height;
		temp.depth = ddsdesc.depth;

		temp.compressed = ddsdesc.compressed;
		temp.iformat = ddsdesc.iformat;
		temp.format = ddsdesc.format;
		temp.type = ddsdesc.type;

		// �����ṹ, ���Դӹ���������һ���ṹ
		*tex = vid_fac_tex_yield;

		memmove(*tex, &temp, sizeof(vid_texture_t));

		// ��ʼ�����ü���Ϊ0
		refcount_init(*tex);

		// ���һ������
		reference(*tex);

		RETURN_TRUE;
	}
	RETURN_FALSE;
}

Bool vid_tex_create_dds_file(vid_texture_t **tex, const wchar_t *file, Bool autoMipmap)
{
	if(tex && file)
	{
		Bool res;
		uint len;
		void *buf = ut_file_read(file, &len);

		res = vid_tex_create_dds(tex, buf, len, autoMipmap);

		ut_file_free(buf);

		return res;
	}
	RETURN_FALSE;
}

Bool vid_tex_release(vid_texture_t **tex)
{
	if(tex)
	{
		// �������Ϊ0
		if(reference_release(*tex))
		{
			// �ͷų�Ա����
			glDeleteTextures(1, &(*tex)->tex_id);
			CHECK_GL_ERR;

			// �ͷŽṹʵ��, �����ù�������
			vid_fac_tex_recycle(*tex);
		}
		*tex = 0;
		RETURN_TRUE;
	}
	RETURN_FALSE;
}

Bool vid_tex_gen_mipmap(vid_texture_t *tex)
{
	if(tex && tex->tex_id)
	{
		glBindTexture(tex->target, tex->tex_id);
		glGenerateMipmap(tex->target);
		CHECK_GL_ERR;
		if(IS_DEBUG) glBindTexture(tex->target, 0);
		RETURN_TRUE;
	}
	RETURN_FALSE;
}

Bool vid_tex_linear_mipmap(vid_texture_t *tex)
{
	if(tex && tex->tex_id)
	{
		glBindTexture(tex->target, tex->tex_id);
		glTexParameteri(tex->target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(tex->target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		if(IS_DEBUG) glBindTexture(tex->target, 0);
		RETURN_TRUE;
	}
	RETURN_FALSE;
}

Bool vid_tex_wrap_edge(vid_texture_t *tex)
{
	if(tex && tex->tex_id)
	{
		glBindTexture(tex->target, tex->tex_id);
		glTexParameteri(tex->target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(tex->target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		if(IS_DEBUG) glBindTexture(tex->target, 0);
		RETURN_TRUE;
	}
	RETURN_FALSE;
}

Bool vid_tex_anisotropy(vid_texture_t *tex, int p)
{
	if(tex && tex->tex_id)
	{
		glBindTexture(tex->target, tex->tex_id);
		glTexParameteri(tex->target, GL_TEXTURE_MAX_ANISOTROPY_EXT, p);
		CHECK_GL_ERR;
		if(IS_DEBUG) glBindTexture(tex->target, 0);
		RETURN_TRUE;
	}
	RETURN_FALSE;
}