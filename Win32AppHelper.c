// 100727
// 100810 检查
// 100820 修改/检查
// 101021 转移wgl函数到此
// 101117 检查

#include "Win32AppHelper.h"
#include <gl/glew.h>
#include <gl/wglew.h>

HWND Win32_CreateBaseWindow(LPCWSTR lpszClassName, LPCWSTR lpszWindowName, WNDPROC lpfnWndProc,
							HICON hIcon, HICON hIconSm, HCURSOR hCursor)
{
	// 新建窗口句柄
	HWND hWnd = NULL;
	// 窗口类结构
	WNDCLASSEXW wcex;

	// 填写窗口类结构
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

	// 注册窗口类
	if(RegisterClassExW(&wcex))
	{
		// 创建主窗口
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

		if(bFullScreen)	// 全屏模式
		{
			nX = nY = 0;
			dwStyle = WS_POPUP | WS_VISIBLE | WS_CLIPSIBLINGS | WS_MINIMIZEBOX;
		}
		else			// 窗口模式
		{
			dwStyle = WS_VISIBLE | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;

			// 可以改变窗口大小
			if(bVariableSize)
			{
				dwStyle |= WS_MAXIMIZEBOX | WS_THICKFRAME;
			}

			// 修正窗口边框尺寸
			AdjustWindowRectEx(&rcWnd, dwStyle, FALSE, 0);
		}

		// 设置新的窗口风格
		if(!SetWindowLongW(hWnd, GWL_STYLE, dwStyle)) return FALSE;

		// 改变窗口位置和尺寸
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

		// 设置为全屏模式
		return ChangeDisplaySettingsW(&devMode, CDS_FULLSCREEN);
	}

	// 设置为窗口模式
	return ChangeDisplaySettingsW(NULL, CDS_RESET);
}

BOOL Win32_InitOpenGL(HWND hWnd, HDC *pDC, HGLRC *pGLRC, BYTE cColorBits, BYTE cDepthBits, BYTE cStencilBits)
{
	if(hWnd && pDC && pGLRC)
	{
		HDC hDC = GetDC(hWnd);
		HGLRC hRC = NULL;

		// 填写像素格式描述表
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

		// 选择并设置像素格式
		int fmt = ChoosePixelFormat(hDC, &pfd);
		if(!SetPixelFormat(hDC, fmt, &pfd))
		{
			ReleaseDC(hWnd, hDC);
			return FALSE;
		}

		// 创建 OpenGL 环境
		hRC = wglCreateContext(hDC);
		if(!hRC)
		{
			ReleaseDC(hWnd, hDC);
			return FALSE;
		}

		// 设置为当前线程
		wglMakeCurrent(hDC, hRC);

		// 初始化 GLEW
		glewInit();

		// 关闭垂直同步
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