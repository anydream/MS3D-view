// 100904
// 100905

#ifndef __DDS_h__
#define __DDS_h__

// DDS 文件头
typedef struct _DDS_HEADER
{
	uint magic;
	uint size;
	uint flags;
	uint height;
	uint width;
	uint pitchOrLinearSize;
	uint depth;
	uint mipMapCount;
	uint reserved[11];
	struct
	{
		uint size;
		uint flags;
		uint fourCC;
		uint bitsPerPixel;
		uint redMask;
		uint greenMask;
		uint blueMask;
		uint alphaMask;
	} pixelFormat;
	struct
	{
		uint caps;
		uint caps2;
	} caps;
	uint reserved2[3];
} DDS_HEADER;

typedef struct _DXTColBlock
{
	ushort col0;
	ushort col1;

	uchar row[4];
} DXTColBlock;

typedef struct _DXT3AlphaBlock
{
	ushort row[4];
} DXT3AlphaBlock;

typedef struct _DXT5AlphaBlock
{
	uchar alpha0;
	uchar alpha1;

	uchar row[6];
} DXT5AlphaBlock;

#define FOURCC(a) ((uint)(a[0]) | (a[1] << 8) | (a[2] << 16) | (a[3] << 24))
#define FOURCC_DXT1		0x31545844
#define FOURCC_DXT3		0x33545844
#define FOURCC_DXT5		0x35545844
#define FOURCC_ABGR16F	0x00000071
#define FOURCC_ABGR32F	0x00000074

#define DDS_FOURCC	0x00000004
#define DDS_ALPHA	0x00000002
#define DDS_LUM		0x00020000
#define DDS_RGB		0x00000040
#define DDS_RGBA	0x00000041

#define DDS_CUBEMAP	0x00000200
#define DDS_VOLUME	0x00200000

// DDS 图像信息
typedef struct _DDSDESC
{
	GLenum	target;
	GLint	iformat;

	uint	levels;
	uint	width, height, depth;

	Bool	compressed;
	uint	components, bpp;
	GLenum	format, type;
} DDSDESC;

#endif