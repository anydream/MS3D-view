// 101117 ���
// 101118 ȥ�� Context
// 110108 ����ܷ�����

#include "AppFramework.h"

APPFRM_CONTEXT	g_AppContext;

//////////////////////////////////////////////////////////////////////////
// ��ʼ��
void App_Init()
{
	memset(&g_AppContext, 0, sizeof(APPFRM_CONTEXT));
}

// ��Ϣ����
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
			// �����ȫ��ģʽ, ��ʧȥ�������Ҫ��С��
			if(g_AppContext.m_bOK && g_AppContext.m_bFullscreen) ShowWindow(g_AppContext.m_hWnd, SW_MINIMIZE);
		}
		break;

	case WM_SIZE:
		{
			if(g_AppContext.m_bOK)
			{
				// ��С��
				if(wParam == SIZE_MINIMIZED)
				{
					// �����ȫ��ģʽ, ��ԭ��ʾģʽ
					if(g_AppContext.m_bFullscreen)
					{
						Win32_ChangeDisplayMode(NULL);
					}
				}
				// �ָ�
				else if(wParam == SIZE_RESTORED)
				{
					// �����ȫ��ģʽ, �����ȫ����ʾ
					if(g_AppContext.m_bFullscreen)
					{
						WIN32_DISPLAYMODE dispMode;
						dispMode.width = g_AppContext.m_nWndWidth;
						dispMode.height = g_AppContext.m_nWndHeight;
						dispMode.bits = 32;

						Win32_ChangeDisplayMode(&dispMode);
					}
				}

				// ���ô���������Ӧ
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
		// ��ֹϵͳ��䴰��
		return 0;
	}
	// ������Ϣ����ϵͳ����
	return DefWindowProcW(hWnd, message, wParam, lParam);
}

// ��������
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

// ���ٴ���
void App_DestroyWindow()
{
	 Win32_DestroyWindow(g_AppContext.m_hWnd);
}

// ������ѭ��
void App_EnterLoop()
{
	BYTE keyTbl[WIN32_KEYNUM], currKeyTbl[WIN32_KEYNUM];
	int keyChange[WIN32_KEYNUM];
	int mouseX, mouseY, curMouseX, curMouseY;
	double tps, startTime, currTime, deltaTime;
	BOOL bFocus = FALSE;

	g_AppContext.m_bOK = TRUE;

	// ��ʼ��Ӧһ�δ�������
	if(g_AppContext.m_pfnResize) g_AppContext.m_pfnResize(g_AppContext.m_nWndWidth, g_AppContext.m_nWndHeight);

	// ��ʼ������
	Win32_GetKeys(keyTbl);

	// ��ʼ��ʱ��
	tps = Win32_GetTicksPerSecond();
	startTime = Win32_GetCurrTime(tps);

	// ��ѭ��
	while(!g_AppContext.m_bExit)
	{
		Win32_DoEvents();

		// �ѻ�ý���
		if(g_AppContext.m_bFocus)
		{
			int changes;

			if(!bFocus)
			{
				bFocus = TRUE;
				// ��ʼ���������
				if(!Win32_GetMousePos(g_AppContext.m_hWnd, &mouseX, &mouseY))
				{
					mouseX = mouseY = 0;
				}
			}

			// ���㰴���ı�
			Win32_GetKeys(currKeyTbl);
			changes = Win32_CompareKeys(currKeyTbl, keyTbl, keyChange);
			memmove(keyTbl, currKeyTbl, sizeof(BYTE) * WIN32_KEYNUM);

			// ���ð�����Ӧ����
			if(g_AppContext.m_pfnKey)
			{
				int i;
				for(i = 0; i < changes; i++)
				{
					g_AppContext.m_pfnKey(keyChange[i], currKeyTbl[keyChange[i]]);
				}
			}

			// �������λ��
			Win32_GetMousePos(g_AppContext.m_hWnd, &curMouseX, &curMouseY);
			// ����λ����Ӧ����
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

		// ����ʱ���
		currTime = Win32_GetCurrTime(tps);
		deltaTime = currTime - startTime;

		// ����֡��Ӧ����
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
