// 100727
// 100810 ���
// 100820 �޸�/���
// 101021 ת��wgl��������
// 101117 ���

#include "Win32AppHelper.h"
#include <gl/glew.h>
#include <gl/wglew.h>

HWND Win32_CreateBaseWindow(LPCWSTR lpszClassName, LPCWSTR lpszWindowName, WNDPROC lpfnWndProc,
							HICON hIcon, HICON hIconSm, HCURSOR hCursor)
{
	// �½����ھ��
	HWND hWnd = NULL;
	// ������ṹ
	WNDCLASSEXW wcex;

	// ��д������ṹ
	wcex.cbSize			= sizeof(WNDCLASSEXW);
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= lpfnWndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= GetModuleHandleW(NULL);
	wcex.hIcon			= hIcon;
	wcex.hCursor		= hCursor;
	wcex.hbrBackground	= NULL;
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= lpszClassName;
	wcex.hIconSm		= hIconSm;

	// ע�ᴰ����
	if(RegisterClassExW(&wcex))
	{
		// ����������
		hWnd = CreateWindowExW(
			0,
			lpszClassName,
			lpszWindowName,
			0,
			0,
			0,
			100,
			100,
			NULL,
			NULL,
			wcex.hInstance,
			NULL);
	}

	return hWnd;
}

BOOL Win32_ModifyWindow(HWND hWnd, BOOL bFullScreen, BOOL bVariableSize, int nX, int nY, int nWidth, int nHeight)
{
	if(hWnd)
	{
		DWORD dwStyle = 0;
		RECT rcWnd;
		rcWnd.left = 0;
		rcWnd.top = 0;
		rcWnd.right = nWidth;
		rcWnd.bottom = nHeight;

		if(bFullScreen)	// ȫ��ģʽ
		{
			nX = nY = 0;
			dwStyle = WS_POPUP | WS_VISIBLE | WS_CLIPSIBLINGS | WS_MINIMIZEBOX;
		}
		else			// ����ģʽ
		{
			dwStyle = WS_VISIBLE | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;

			// ���Ըı䴰�ڴ�С
			if(bVariableSize)
			{
				dwStyle |= WS_MAXIMIZEBOX | WS_THICKFRAME;
			}

			// �������ڱ߿�ߴ�
			AdjustWindowRectEx(&rcWnd, dwStyle, FALSE, 0);
		}

		// �����µĴ��ڷ��
		if(!SetWindowLongW(hWnd, GWL_STYLE, dwStyle)) return FALSE;

		// �ı䴰��λ�úͳߴ�
		if(!SetWindowPos(hWnd, NULL, nX, nY, rcWnd.right - rcWnd.left, rcWnd.bottom - rcWnd.top, 0)) return FALSE;

		return TRUE;
	}

	return FALSE;
}

void Win32_RefreshWindow(HWND hWnd)
{
	if(hWnd)
	{
		ShowWindow(hWnd, SW_SHOW);
		SetFocus(hWnd);
		SetForegroundWindow(hWnd);
		FlashWindow(hWnd, FALSE);
	}
}

void Win32_DestroyWindow(HWND hWnd)
{
	if(hWnd)
	{
		WCHAR lpszClassName[MAX_PATH] = { 0 };
		GetClassNameW(hWnd, lpszClassName, MAX_PATH);
		DestroyWindow(hWnd);
		UnregisterClassW(lpszClassName, GetModuleHandleW(NULL));
	}
}

void Win32_DoEvents()
{
	MSG msg;
	if(PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}
}

//////////////////////////////////////////////////////////////////////////
double Win32_GetTicksPerSecond()
{
	LARGE_INTEGER ticksPerSecond;
	if(!QueryPerformanceFrequency(&ticksPerSecond)) return 0.0;
	return 1.0 / (double)ticksPerSecond.QuadPart;
}

double Win32_GetCurrTime(double tps)
{
	LARGE_INTEGER currTime;
	if(!QueryPerformanceCounter(&currTime)) return 0.0;
	return (double)currTime.QuadPart * tps;
}

//////////////////////////////////////////////////////////////////////////
BOOL Win32_GetKeys(BYTE *tbl)
{
	if(GetKeyboardState(tbl))
	{
		int i;
		for(i = 0; i < WIN32_KEYNUM; i++)
			tbl[i] = (tbl[i] & 0x80) ? 1 : 0;
		return TRUE;
	}
	return FALSE;
}

int Win32_CompareKeys(const BYTE *tblCur, const BYTE *tblLast, int *idx)
{
	if(tblCur && tblLast && idx)
	{
		int p = 0;
		int i;
		for(i = 0; i < WIN32_KEYNUM; i++)
		{
			if(tblCur[i] != tblLast[i])
			{
				idx[p] = i;
				p++;
			}
		}
		return p;
	}
	return 0;
}

BOOL Win32_GetMousePos(HWND hWnd, int *x, int *y)
{
	if(hWnd)
	{
		POINT point;

		if(!GetCursorPos(&point)) return FALSE;
		if(!ScreenToClient(hWnd, &point)) return FALSE;

		if(x) *x = point.x;
		if(y) *y = point.y;

		return TRUE;
	}

	return FALSE;
}

BOOL Win32_ChangeDisplayMode(const WIN32_DISPLAYMODE *dispMode)
{
	if(dispMode)
	{
		DEVMODEW devMode;
		memset(&devMode, 0, sizeof(DEVMODEW));

		devMode.dmSize = sizeof(DEVMODEW);
		devMode.dmPelsWidth = dispMode->width;
		devMode.dmPelsHeight = dispMode->height;
		devMode.dmBitsPerPel = dispMode->bits;
		devMode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// ����Ϊȫ��ģʽ
		return ChangeDisplaySettingsW(&devMode, CDS_FULLSCREEN);
	}

	// ����Ϊ����ģʽ
	return ChangeDisplaySettingsW(NULL, CDS_RESET);
}

BOOL Win32_InitOpenGL(HWND hWnd, HDC *pDC, HGLRC *pGLRC, BYTE cColorBits, BYTE cDepthBits, BYTE cStencilBits)
{
	if(hWnd && pDC && pGLRC)
	{
		HDC hDC = GetDC(hWnd);
		HGLRC hRC = NULL;

		// ��д���ظ�ʽ������
		PIXELFORMATDESCRIPTOR pfd =
		{
			sizeof(PIXELFORMATDESCRIPTOR),
			1,
			PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
			PFD_TYPE_RGBA,
			cColorBits,
			0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0,
			cDepthBits,
			cStencilBits,
			0,
			PFD_MAIN_PLANE,
			0,
			0, 0, 0
		};

		// ѡ���������ظ�ʽ
		int fmt = ChoosePixelFormat(hDC, &pfd);
		if(!SetPixelFormat(hDC, fmt, &pfd))
		{
			ReleaseDC(hWnd, hDC);
			return FALSE;
		}

		// ���� OpenGL ����
		hRC = wglCreateContext(hDC);
		if(!hRC)
		{
			ReleaseDC(hWnd, hDC);
			return FALSE;
		}

		// ����Ϊ��ǰ�߳�
		wglMakeCurrent(hDC, hRC);

		// ��ʼ�� GLEW
		glewInit();

		// �رմ�ֱͬ��
		wglSwapIntervalEXT(GL_FALSE);

		*pDC = hDC;
		*pGLRC = hRC;

		return TRUE;
	}

	return FALSE;
}

void Win32_FreeOpenGL(HWND hWnd, HDC hDC, HGLRC hGLRC)
{
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(hGLRC);
	ReleaseDC(hWnd, hDC);
}