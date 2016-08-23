// 101117 检查
// 101118 去除 Context
// 110108 鼠标跑飞问题

#include "AppFramework.h"

APPFRM_CONTEXT	g_AppContext;

//////////////////////////////////////////////////////////////////////////
// 初始化
void App_Init()
{
	memset(&g_AppContext, 0, sizeof(APPFRM_CONTEXT));
}

// 消息处理
LRESULT CALLBACK App_MsgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case WM_SETFOCUS:
		g_AppContext.m_bFocus = TRUE;
		break;

	case WM_KILLFOCUS:
		{
			g_AppContext.m_bFocus = FALSE;
			// 如果是全屏模式, 则失去焦点后需要最小化
			if(g_AppContext.m_bOK && g_AppContext.m_bFullscreen) ShowWindow(g_AppContext.m_hWnd, SW_MINIMIZE);
		}
		break;

	case WM_SIZE:
		{
			if(g_AppContext.m_bOK)
			{
				// 最小化
				if(wParam == SIZE_MINIMIZED)
				{
					// 如果是全屏模式, 则还原显示模式
					if(g_AppContext.m_bFullscreen)
					{
						Win32_ChangeDisplayMode(NULL);
					}
				}
				// 恢复
				else if(wParam == SIZE_RESTORED)
				{
					// 如果是全屏模式, 则进入全屏显示
					if(g_AppContext.m_bFullscreen)
					{
						WIN32_DISPLAYMODE dispMode;
						dispMode.width = g_AppContext.m_nWndWidth;
						dispMode.height = g_AppContext.m_nWndHeight;
						dispMode.bits = 32;

						Win32_ChangeDisplayMode(&dispMode);
					}
				}

				// 调用窗口缩放响应
				if(g_AppContext.m_pfnResize) g_AppContext.m_pfnResize(LOWORD(lParam), HIWORD(lParam));
			}
		}
		break;

	case WM_CLOSE:
		{
			if(g_AppContext.m_pfnExit)
			{
				if(!g_AppContext.m_pfnExit()) return 0;
			}

			//! PostMessageW(hWnd, WM_QUIT, 0, 0);
			App_ExitLoop();
		}
		break;

	case WM_ERASEBKGND:
		// 禁止系统填充窗口
		return 0;
	}
	// 其他消息交回系统处理
	return DefWindowProcW(hWnd, message, wParam, lParam);
}

// 创建窗口
HWND App_CreateWindow(const wchar_t *wndName, int x, int y, int width, int height)
{
	if(wndName)
	{
		g_AppContext.m_hWnd = Win32_CreateBaseWindow(L"AppFrmCls", wndName, App_MsgProc, NULL, NULL, LoadCursor(NULL, IDC_ARROW));

		if(g_AppContext.m_hWnd)
		{
			g_AppContext.m_nWndX = x;
			g_AppContext.m_nWndY = y;
			g_AppContext.m_nWndWidth = width;
			g_AppContext.m_nWndHeight = height;

			App_Fullscreen(g_AppContext.m_bFullscreen);
			Win32_RefreshWindow(g_AppContext.m_hWnd);

			return g_AppContext.m_hWnd;
		}
	}

	return NULL;
}

// 销毁窗口
void App_DestroyWindow()
{
	 Win32_DestroyWindow(g_AppContext.m_hWnd);
}

// 进入主循环
void App_EnterLoop()
{
	BYTE keyTbl[WIN32_KEYNUM], currKeyTbl[WIN32_KEYNUM];
	int keyChange[WIN32_KEYNUM];
	int mouseX, mouseY, curMouseX, curMouseY;
	double tps, startTime, currTime, deltaTime;
	BOOL bFocus = FALSE;

	g_AppContext.m_bOK = TRUE;

	// 开始响应一次窗口缩放
	if(g_AppContext.m_pfnResize) g_AppContext.m_pfnResize(g_AppContext.m_nWndWidth, g_AppContext.m_nWndHeight);

	// 初始化按键
	Win32_GetKeys(keyTbl);

	// 初始化时间
	tps = Win32_GetTicksPerSecond();
	startTime = Win32_GetCurrTime(tps);

	// 主循环
	while(!g_AppContext.m_bExit)
	{
		Win32_DoEvents();

		// 已获得焦点
		if(g_AppContext.m_bFocus)
		{
			int changes;

			if(!bFocus)
			{
				bFocus = TRUE;
				// 初始化鼠标坐标
				if(!Win32_GetMousePos(g_AppContext.m_hWnd, &mouseX, &mouseY))
				{
					mouseX = mouseY = 0;
				}
			}

			// 计算按键改变
			Win32_GetKeys(currKeyTbl);
			changes = Win32_CompareKeys(currKeyTbl, keyTbl, keyChange);
			memmove(keyTbl, currKeyTbl, sizeof(BYTE) * WIN32_KEYNUM);

			// 调用按键响应函数
			if(g_AppContext.m_pfnKey)
			{
				int i;
				for(i = 0; i < changes; i++)
				{
					g_AppContext.m_pfnKey(keyChange[i], currKeyTbl[keyChange[i]]);
				}
			}

			// 计算鼠标位移
			Win32_GetMousePos(g_AppContext.m_hWnd, &curMouseX, &curMouseY);
			// 调用位移响应函数
			if(g_AppContext.m_pfnMotion)
			{
				if(mouseX != curMouseX || mouseY != curMouseY)
				{
					g_AppContext.m_pfnMotion(curMouseX, curMouseY, curMouseX - mouseX, curMouseY - mouseY);
				}
			}
			mouseX = curMouseX;
			mouseY = curMouseY;
		}
		else
		{
			if(bFocus != FALSE) bFocus = FALSE;
		}

		// 计算时间差
		currTime = Win32_GetCurrTime(tps);
		deltaTime = currTime - startTime;

		// 调用帧响应函数
		g_AppContext.m_pfnDisplay(deltaTime);

		startTime = currTime;
	}
}

void App_ExitLoop()
{
	 g_AppContext.m_bExit = TRUE;
}

void App_Fullscreen(BOOL bFull)
{
	if(bFull)
	{
		WIN32_DISPLAYMODE dispMode;
		dispMode.width = g_AppContext.m_nWndWidth;
		dispMode.height = g_AppContext.m_nWndHeight;
		dispMode.bits = 32;

		Win32_ModifyWindow(g_AppContext.m_hWnd, TRUE, FALSE, 0, 0, g_AppContext.m_nWndWidth, g_AppContext.m_nWndHeight);
		if(g_AppContext.m_bFullscreen != bFull) Win32_ChangeDisplayMode(&dispMode);
	}
	else
	{
		if(g_AppContext.m_bFullscreen != bFull) Win32_ChangeDisplayMode(NULL);
		Win32_ModifyWindow(g_AppContext.m_hWnd, FALSE, g_AppContext.m_pfnResize ? TRUE : FALSE,
			g_AppContext.m_nWndX, g_AppContext.m_nWndY, g_AppContext.m_nWndWidth, g_AppContext.m_nWndHeight);
	}

	g_AppContext.m_bFullscreen = bFull;
}

BOOL App_IsFullscreen()
{
	return g_AppContext.m_bFullscreen;
}

void App_DisplayFunc(APP_PFNDISPLAY display)
{
	if(display) g_AppContext.m_pfnDisplay = display;
}

void App_ResizeFunc(APP_PFNRESIZE resize)
{
	if(resize) g_AppContext.m_pfnResize = resize;
}

void App_KeyFunc(APP_PFNKEY key)
{
	if(key) g_AppContext.m_pfnKey = key;
}

void App_MotionFunc(APP_PFNMOTION motion)
{
	if(motion) g_AppContext.m_pfnMotion = motion;
}

void App_ExitFunc(APP_PFNEXIT ex)
{
	if(ex) g_AppContext.m_pfnExit = ex;
}

BOOL App_MousePos(int *x, int *y)
{
	return Win32_GetMousePos(g_AppContext.m_hWnd, x, y);
}
