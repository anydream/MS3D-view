// 100820
// 101117 ���

#ifndef __AppFramework_h__
#define __AppFramework_h__

#include "Win32AppHelper.h"

#ifdef __cplusplus
extern "C"
{
#endif

// ֡��Ӧ�ص�
typedef void (*APP_PFNDISPLAY)(double delta);
// �������Żص�
typedef void (*APP_PFNRESIZE)(int width, int height);
// �����ص�
typedef void (*APP_PFNKEY)(int key, BYTE state);
// �ƶ����ص�
typedef void (*APP_PFNMOTION)(int x, int y, int xoff, int yoff);
// �˳�����ص�
typedef BOOL (*APP_PFNEXIT)();

// ��������Ϣ
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

// ��ʼ�������Ϣ
void App_Init();
// ��������
HWND App_CreateWindow(const wchar_t *wndName, int x, int y, int width, int height);
// ���ٴ���
void App_DestroyWindow();
// ������Ϣѭ��
void App_EnterLoop();
// �˳���Ϣѭ��
void App_ExitLoop();
// ȫ������
void App_Fullscreen(BOOL bFull);
// �Ƿ�ȫ��
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