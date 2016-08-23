// 101231 重构
// 110128 拷贝到当前工程

#include <stdio.h>

#include "Win32AppHelper.h"
#include "AppFramework.h"

#include "vid.h"

HWND			g_hWnd = NULL;
HDC				g_hDC = NULL;
HGLRC			g_hGLRC = NULL;
unsigned int	g_ScreenWidth = 1024;
unsigned int	g_ScreenHeight = 768;

void on_init();
void on_destroy();
void on_display(double deltaTime);
void on_key(int key, BYTE state);
void on_motion(int x, int y, int xoff, int yoff);
void on_reshape(int width, int height);
BOOL on_exit();

//////////////////////////////////////////////////////////////////////////
int app_entry(int argc, wchar_t **argv)
{
	App_Init();

	App_DisplayFunc(on_display);
	App_ResizeFunc(on_reshape);
	App_KeyFunc(on_key);
	App_MotionFunc(on_motion);
	App_ExitFunc(on_exit);

	g_hWnd = App_CreateWindow(L"AnyDream OpenGL \x6e32\x67d3\x7cfb\x7edf", 10, 10, g_ScreenWidth, g_ScreenHeight);
	if(g_hWnd)
	{
		if(Win32_InitOpenGL(g_hWnd, &g_hDC, &g_hGLRC, 32, 32, 0))
		{
			// 输出显卡信息
			const GLubyte *str;
			str = glGetString(GL_VENDOR);
			printf("vendor: %s\n", str);

			glClearColor(0.5, 0.5, 0.5, 1.0);

			// 设置线样式
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glEnable(GL_LINE_SMOOTH);
			glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

			//! 开启裁剪
			//glEnable(GL_CULL_FACE);

			//////////////////////////////////////////////////////////////////////////
			on_init();

			App_EnterLoop();

			on_destroy();

			Win32_FreeOpenGL(g_hWnd, g_hDC, g_hGLRC);
		}
		App_DestroyWindow();
	}
	else MessageBoxW(NULL, L"\x521b\x5efa\x7a97\x53e3\x5931\x8d25\x21", NULL, MB_OK | MB_ICONERROR);
	return 0;
}
