// 110128

#ifndef __VidDebug_h__
#define __VidDebug_h__

#ifdef __cplusplus
extern "C"
{
#endif
//////////////////////////////////////////////////////////////////////////
// 输出 GL 函数调用错误信息
#ifdef _DEBUG
#  include <stdio.h>
void __vid_error(int n);
#  define IS_DEBUG		True
#  define CHECK_CG_ERR	{ if(cgGetError() != CG_NO_ERROR) { __vid_error(0); return False; } }
#  define CHECK_GL_ERR	{ if(glGetError() != GL_NO_ERROR) { __vid_error(1); return False; } }
#  define RETURN_FALSE	{ __vid_error(2); return False; }
#  define RETURN_TRUE	{ return True; }
#else
#  define IS_DEBUG		False
#  define CHECK_CG_ERR
#  define CHECK_GL_ERR
#  define RETURN_FALSE	{ return False; }
#  define RETURN_TRUE	{ return True; }
#endif

#ifdef __cplusplus
}
#endif

#endif