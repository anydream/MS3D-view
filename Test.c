
#include "Win32AppHelper.h"
#include "AppFramework.h"
#include "vid.h"
#include "mdl_test.h"

extern HDC g_hDC;

mat4 g_matProj;

float g_angleX = 180, g_angleY = 0;
Bool g_press = False;

Bool g_bSleep = True;
Bool g_bFPS = True;
double g_Rate = 1.0;
double g_Orien = 1.0;

extern vid_system_t g_vid_system;

//////////////////////////////////////////////////////////////////////////
void CGENTRY on_cg_err(void)
{
	CGerror err;
	const char *str = cgGetLastErrorString(&err);
	if(err != CG_NO_ERROR)
	{
		printf("cg err[%d]: %s\n", err, str);
		if(err == CG_COMPILER_ERROR)
			printf("cg compiler err: \n%s\n", cgGetLastListing(g_vid_system.cg_context));
	}
}

void on_init()
{
	{
		printf("- ������ ��תģ��\n");
		printf("- �ո�� ���ƶ����Ĳ���/��ͣ\n");
		printf("- ������ ���Ͷ����Ĳ�������, ��ֵʱ�ɵ���\n");
		printf("- ����Ҽ� ��߶����Ĳ�������\n");
		printf("- S�� ���� CPU ˯��\n");
		printf("- F�� ȫ��/�����л�\n");
		printf("- P�� FPS ����ʾ/����\n");
	}
	// ����ϵͳ
	{
		glLineWidth(5.0);
		// ���ô�����ص�
		cgSetErrorCallback(on_cg_err);

		// ��ʼ��ϵͳ
		vid_sys_create();
		// ϵͳ�����������
		vid_sys_manage_texture_param(True);
		// �ӳٲ���
		vid_sys_deferred_param(True);
	}

	//...
	mdl_test_init();
}

void on_destroy()
{
	mdl_test_free();
	vid_sys_destroy();
}

double g_cnttime = 0.0;
uint g_cntfrms = 0;

void on_display(double deltaTime)
{
	if(g_bSleep) Sleep(1);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	{
		mat4 mat, mat2, mat3;
		vec3 up = { 0, 1, 0 };
		vec3 at = { 0, 0, 0 };
		vec3 cross, vout;

		camFirstPersonD(&g_angleX, &g_angleY, up, cross, vout);
		matLookAtLH(vout, at, up, mat);

		matTranslate(0, -100, 0, mat2);
		matScale(0.0034, 0.0034, 0.0034, mat3);
		matMul(mat2, mat3, mat2);

		matMul(mat2, mat, mat);
		matMulProj(mat, g_matProj, mat3);

		mdl_test_draw(deltaTime * g_Rate * g_Orien, mat3, mat);
	}
	SwapBuffers(g_hDC);

	{
		g_cntfrms++;
		g_cnttime += deltaTime;
		
		if(g_cnttime >= 1.0)
		{
			if(g_bFPS) printf("FPS: %d\n", g_cntfrms);
			g_cnttime = 0.0;
			g_cntfrms = 0;
		}
	}
}

void on_key(int key, BYTE state)
{
	if(key == 1)
	{
		if(state) g_press = True;
		else g_press = False;
	}
	if(state && key == 'F')
	{
		App_Fullscreen(!App_IsFullscreen());
	}
	else if(state && key == 27)
	{
		App_ExitLoop();
	}
	else if(state && key == 'S')
	{
		g_bSleep = !g_bSleep;
	}
	else if(state && key == 'P')
	{
		g_bFPS = !g_bFPS;
	}
	else if(state && key == ' ')
	{
		if(g_Rate == 0.0) g_Rate = 1.0;
		else g_Rate = 0.0;
	}
	else if(state && key == 37)
	{
		g_Orien -= 0.1;
	}
	else if(state && key == 39)
	{
		g_Orien += 0.1;
	}
	//printf("%d, %d\n", key, state);
}

void on_motion(int x, int y, int xoff, int yoff)
{
	if(g_press)
	{
		g_angleX += xoff * 0.2f;
		g_angleY -= yoff * 0.2f;
	}
	//printf("%d, %d\n", xoff, yoff);
}

void on_reshape(int width, int height)
{
	if(height == 0) height = 1;
	// ...
	glViewport(0, 0, width, height);

	// ���㴰�ڵ���۱���
	matPerspectiveLHD(45.0f, (float)width/(float)height, 0.1f, 900.0f, g_matProj);
}

BOOL on_exit()
{
	return TRUE;
}
