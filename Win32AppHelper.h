// 090725
// 100210
// 100417
// 100613
// 100810
// 100818 OpenGL
// 100820 修改
// 101117 检查

#ifndef __Win32AppHelper_h__
#define __Win32AppHelper_h__

#ifndef UNICODE
#  define UNICODE
#endif
#ifndef _UNICODE
#  define _UNICODE
#endif
#include <windows.h>

#ifdef __cplusplus
extern "C"
{
#endif

// 按键个数
#define WIN32_KEYNUM	256
// 按键状态
#define WIN32_KEYSTATE(__key)	((GetAsyncKeyState(__key) & 0x8000) ? 1 : 0)

// 显示模式
typedef struct _WIN32_DISPLAYMODE
{
	unsigned int width;		// 宽度
	unsigned int height;	// 高度
	unsigned int bits;		// 颜色位
} WIN32_DISPLAYMODE;

//////////////////////////////////////////////////////////////////////////
// 创建基本窗口
HWND	Win32_CreateBaseWindow(LPCWSTR lpszClassName, LPCWSTR lpszWindowName, WNDPROC lpfnWndProc,
								HICON hIcon, HICON hIconSm, HCURSOR hCursor);
// 修改窗口属性
BOOL	Win32_ModifyWindow(HWND hWnd, BOOL bFullScreen, BOOL bVariableSize,
						   int nX, int nY, int nWidth, int nHeight);
// 刷新窗口
void	Win32_RefreshWindow(HWND hWnd);
// 销毁窗口
void	Win32_DestroyWindow(HWND hWnd);

// 进行一次消息处理
void	Win32_DoEvents();

// 获得时间频率
double	Win32_GetTicksPerSecond();
// 获得当前时间(秒)
double	Win32_GetCurrTime(double tps);

// 获得当前键盘状态表
BOOL	Win32_GetKeys(BYTE *tbl);
// 比较两个键盘状态表
int		Win32_CompareKeys(const BYTE *tblCur, const BYTE *tblLast, int *idx);
// 获得鼠标位置
BOOL	Win32_GetMousePos(HWND hWnd, int *x, int *y);

// 改变显示模式
BOOL	Win32_ChangeDisplayMode(const WIN32_DISPLAYMODE *dispMode);

// 初始化 OpenGL
BOOL	Win32_InitOpenGL(HWND hWnd, HDC *pDC, HGLRC *pGLRC, BYTE cColorBits, BYTE cDepthBits, BYTE cStencilBits);
// 释放 OpenGL
void	Win32_FreeOpenGL(HWND hWnd, HDC hDC, HGLRC hGLRC);

#ifdef __cplusplus
}
#endif

#endif