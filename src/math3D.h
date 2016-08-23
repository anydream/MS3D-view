// 100822
// 100829
// 101123 改进完毕
// 110130 double 转 float
// 110216 改进

#ifndef __Math3D_h__
#define __Math3D_h__

#ifdef __cplusplus
extern "C"
{
#endif

// 圆周率
#define PI			((float)3.14159265358979323846)

#define to_radian	(PI / 180.0f)
#define to_degree	(180.0f / PI)

#define vecAdd3(_v1, _v2, _vout)\
	((_vout)[0] = (_v1)[0] + (_v2)[0],\
	(_vout)[1] = (_v1)[1] + (_v2)[1],\
	(_vout)[2] = (_v1)[2] + (_v2)[2])

#define vecSub3(_v1, _v2, _vout)\
	((_vout)[0] = (_v1)[0] - (_v2)[0],\
	(_vout)[1] = (_v1)[1] - (_v2)[1],\
	(_vout)[2] = (_v1)[2] - (_v2)[2])

#define vecMulNum3(_v, _n, _vout)\
	((_vout)[0] = (_n) * (_v)[0],\
	(_vout)[1] = (_n) * (_v)[1],\
	(_vout)[2] = (_n) * (_v)[2])

#define vecCopy3(_v, _vout)\
	((_vout)[0] = (_v)[0],\
	(_vout)[1] = (_v)[1],\
	(_vout)[2] = (_v)[2])

#define vecDot3(_x, _y)	((_x)[0] * (_y)[0] + (_x)[1] * (_y)[1] + (_x)[2] * (_y)[2])
#define vecDot4(_x, _y)	((_x)[0] * (_y)[0] + (_x)[1] * (_y)[1] + (_x)[2] * (_y)[2] + (_x)[3] * (_y)[3])

typedef float vec2[2], vec3[3], vec4[4], quat[4], mat4[16];
typedef unsigned short half, half2[2], half3[3], half4[4];

half float_to_half(float real);

void vecNormalize(const vec3 v, vec3 vout);
void vecCross(const vec3 v1, const vec3 v2, vec3 vout);

void vecRotate(const vec3 v, const mat4 m, vec3 vout);
void vecInvRotate(const vec3 v, const mat4 m, vec3 vout);
void vecTransform(const vec3 v, const mat4 m, vec3 vout);
void vecInvTransform(const vec3 v, const mat4 m, vec3 vout);

// Hermite 线性插值
void vecHermite(const vec3 p, const vec3 q, float t, vec3 h);

void matIdentity(mat4 m);

void matPerspectiveLHR(float fieldOfView,
					   float aspectRatio,
					   float zNear, float zFar,
					   mat4 m);

void matPerspectiveRHR(float fieldOfView,
					   float aspectRatio,
					   float zNear, float zFar,
					   mat4 m);

void matPerspectiveLHD(float fieldOfView,
					   float aspectRatio,
					   float zNear, float zFar,
					   mat4 m);

void matPerspectiveRHD(float fieldOfView,
					   float aspectRatio,
					   float zNear, float zFar,
					   mat4 m);

void matLookAtLH(const vec3 eye, const vec3 at, const vec3 up, mat4 m);
void matLookAtRH(const vec3 eye, const vec3 at, const vec3 up, mat4 m);

void matMulProj(const mat4 src1, const mat4 src2, mat4 dst);
void matMul(const mat4 src1, const mat4 src2, mat4 dst);

void matAngleR(const vec3 radians, mat4 m);
//*坐标轴必须为单位向量, 否则结果不正确
void matRotateR2(float radian, const vec3 axis, mat4 m);
void matRotateR(float radian, float ax, float ay, float az, mat4 m);
void matRotateD2(float degree, const vec3 axis, mat4 m);
void matRotateD(float degree, float ax, float ay, float az, mat4 m);

void matTranslate2(const vec3 axis, mat4 m);
void matTranslate(float x, float y, float z, mat4 m);

void matScale2(const vec3 axis, mat4 m);
void matScale(float x, float y, float z, mat4 m);

void matInverse(const mat4 mat, mat4 m);

// 四元数和位置 合并成矩阵
void matQuatTrans(const quat quaternion, const vec3 trans, mat4 matrix);
void matQuat(const quat quaternion, mat4 m);

void quaMatrix(const mat4 mat, quat quaternion);
void quaAngleR(const vec3 radians, quat quaternion);
void quaSlerp(const quat p, const quat q, float t, quat quaternion);

void tangentSpaceLH(const vec3 p1, const vec3 p2, const vec3 p3,
					const vec2 texc1, const vec2 texc2, const vec2 texc3,
					vec3 out_normal, vec3 out_tangent, vec3 out_binormal);

void camFirstPersonD(float *ax, float *ay, const vec3 up, vec3 cross, vec3 vout);

#ifdef __cplusplus
}
#endif

#endif