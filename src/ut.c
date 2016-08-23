// 101003
// 101012 检查

#include "ut.h"
#include <stdio.h>

// 文件函数的定义
#if (!defined _MSC_VER) || (_MSC_VER <= 1200)
#  define fopen_p(_fptr, _path, _mode)	((_fptr) = _wfopen(_path, _mode))
#else
#  define fopen_p(_fptr, _path, _mode)	(_wfopen_s(&(_fptr), _path, _mode))
#endif
//////////////////////////////////////////////////////////////////////////
uint g_utRandomSeed = 0;

void ut_free(void *p)
{
	if(p) free(p);
}

void *ut_file_read(const wchar_t *file, uint *len)
{
	if(file)
	{
		FILE *fp;

		fopen_p(fp, file, L"rb");
		if(fp)
		{
			void *buf = 0;
			uint flen = 0;

			fseek(fp, 0L, SEEK_END);
			flen = ftell(fp);
			fseek(fp, 0L, SEEK_SET);

			if(flen)
			{
				buf = ut_alloc(flen);
				if(buf) fread(buf, flen, 1, fp);
			}

			fclose(fp);

			if(len) *len = flen;

			return buf;
		}
	}
	return 0;
}

void ut_file_free(void *buf)
{
	ut_free(buf);
}

void ut_srand(uint seed)
{
	g_utRandomSeed = seed;
}

int ut_rand()
{
	g_utRandomSeed = g_utRandomSeed * 214013L + 2531011L;
	return (g_utRandomSeed >> 16) & 0x7fff;
}

// 随机数
int ut_random(int minimum, int maximum)
{
	int tmp = MAX(maximum, minimum);
	minimum = MIN(minimum, maximum);
	maximum = tmp;

	tmp = maximum - minimum + 1;
	if(tmp != 0)
	{
		tmp = ut_rand() % tmp + minimum;
		if(tmp < minimum) tmp = minimum;
		else if(tmp > maximum) tmp = maximum;
		return tmp;
	}
	return 0;
}