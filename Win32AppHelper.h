// 090725
// 100210
// 100417
// 100613
// 100810
// 100818 OpenGL
// 100820 �޸�
// 101117 ���

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

// ��������
#define WIN32_KEYNUM	256
// ����״̬
#define WIN32_KEYSTATE(__key)	((GetAsyncKeyState(__key) & 0x8000) ? 1 : 0)

// ��ʾģʽ
typedef struct _WIN32_DISPLAYMODE
{
	unsigned int width;		// ���
	unsigned int height;	// �߶�
	unsigned int bits;		// ��ɫλ
} WIN32_DISPLAYMODE;

//////////////////////////////////////////////////////////////////////////
// ������������
HWND	Win32_CreateBaseWindow(LPCWSTR lpszClassName, LPCWSTR lpszWindowName, WNDPROC lpfnWndProc,
								HICON hIcon, HICON hIconSm, HCURSOR hCursor);
// �޸Ĵ�������
BOOL	Win32_ModifyWindow(HWND hWnd, BOOL bFullScreen, BOOL bVariableSize,
						   int nX, int nY, int nWidth, int nHeight);
// ˢ�´���
void	Win32_RefreshWindow(HWND hWnd);
// ���ٴ���
void	Win32_DestroyWindow(HWND hWnd);

// ����һ����Ϣ����
void	Win32_DoEvents();

// ���ʱ��Ƶ��
double	Win32_GetTicksPerSecond();
// ��õ�ǰʱ��(��)
double	Win32_GetCurrTime(double tps);

// ��õ�ǰ����״̬��
BOOL	Win32_GetKeys(BYTE *tbl);
// �Ƚ���������״̬��
int		Win32_CompareKeys(const BYTE *tblCur, const BYTE *tblLast, int *idx);
// ������λ��
BOOL	Win32_GetMousePos(HWND hWnd, int *x, int *y);

// �ı���ʾģʽ
BOOL	Win32_ChangeDisplayMode(const WIN32_DISPLAYMODE *dispMode);

// ��ʼ�� OpenGL
BOOL	Win32_InitOpenGL(HWND hWnd, HDC *pDC, HGLRC *pGLRC, BYTE cColorBits, BYTE cDepthBits, BYTE cStencilBits);
// �ͷ� OpenGL
void	Win32_FreeOpenGL(HWND hWnd, HDC hDC, HGLRC hGLRC);

#ifdef __cplusplus
}
#endif

#endif