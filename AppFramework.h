// 100820
// 101117 检查

#ifndef __AppFramework_h__
#define __AppFramework_h__

#include "Win32AppHelper.h"

#ifdef __cplusplus
extern "C"
{
#endif

// 帧响应回调
typedef void (*APP_PFNDISPLAY)(double delta);
// 窗口缩放回调
typedef void (*APP_PFNRESIZE)(int width, int height);
// 按键回调
typedef void (*APP_PFNKEY)(int key, BYTE state);
// 移动鼠标回调
typedef void (*APP_PFNMOTION)(int x, int y, int xoff, int yoff);
// 退出程序回调
typedef BOOL (*APP_PFNEXIT)();

// 程序框架信息
typedef struct _APPFRM_CONTEXT
{
	BOOL	m_bOK;
	HWND	m_hWnd;
	BOOL	m_bExit;
	BOOL	m_bFocus;
	BOOL	m_bFullscreen;

	int		m_nWndX, m_nWndY, m_nWndWidth, m_nWndHeight;

	APP_PFNDISPLAY	m_pfnDisplay;
	APP_PFNRESIZE	m_pfnResize;
	APP_PFNKEY		m_pfnKey;
	APP_PFNMOTION	m_pfnMotion;
	APP_PFNEXIT		m_pfnExit;
} APPFRM_CONTEXT;

// 初始化框架信息
void App_Init();
// 创建窗口
HWND App_CreateWindow(const wchar_t *wndName, int x, int y, int width, int height);
// 销毁窗口
void App_DestroyWindow();
// 进入消息循环
void App_EnterLoop();
// 退出消息循环
void App_ExitLoop();
// 全屏设置
void App_Fullscreen(BOOL bFull);
// 是否全屏
BOOL App_IsFullscreen();

void App_DisplayFunc(APP_PFNDISPLAY display);
void App_ResizeFunc(APP_PFNRESIZE resize);
void App_KeyFunc(APP_PFNKEY key);
void App_MotionFunc(APP_PFNMOTION motion);
void App_ExitFunc(APP_PFNEXIT ex);

BOOL App_MousePos(int *x, int *y);

#ifdef __cplusplus
}
#endif

#endif