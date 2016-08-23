// 101230

#ifndef __MDL_Test_h__
#define __MDL_Test_h__

#include "vid.h"
#include "math3D.h"

#ifdef __cplusplus
extern "C"
{
#endif

void mdl_test_init();
void mdl_test_free();
void mdl_test_draw(double deltaTime, const mat4 matWVP, const mat4 matWorldView);

#ifdef __cplusplus
}
#endif

#endif