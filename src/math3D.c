// 100829
// 101005
// 101007
// 101123 改进完毕

#include "math3D.h"
#include "types.h"
#include <math.h>

#define sqrtf(_x)    ((float)sqrt((double)(_x)))
#define sinf(_x)     ((float)sin((double)(_x)))
#define cosf(_x)     ((float)cos((double)(_x)))
#define acosf(_x)    ((float)acos((double)(_x)))

// 110216
half float_to_half(float real)
{
	uint res;

	uint ival = ((uint*)(&real))[0];
	uint sign = (ival & 0x80000000U) >> 16U;
	ival = ival & 0x7FFFFFFFU;

	if(ival > 0x47FFEFFFU) res = 0x7FFFU;
	else
	{
		if(ival < 0x38800000U)
		{
			uint shift = 113U - (ival >> 23U);
			ival = (0x800000U | (ival & 0x7FFFFFU)) >> shift;
		}
		else
		{
			ival += 0xC8000000U;
		}

		res = ((ival + 0x0FFFU + ((ival >> 13U) & 1U)) >> 13U) & 0x7FFFU; 
	}
	return (half)(res | sign);
}

void vecNormalize(const vec3 v, vec3 vout)
{
	float mag = 1.0f / sqrtf(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);

	vout[0] = v[0] * mag;
	vout[1] = v[1] * mag;
	vout[2] = v[2] * mag;
}

void vecCross(const vec3 v1, const vec3 v2, vec3 vout)
{
	vec3 tmp;

	tmp[0] = v1[1] * v2[2] - v1[2] * v2[1];
	tmp[1] = v1[2] * v2[0] - v1[0] * v2[2];
	tmp[2] = v1[0] * v2[1] - v1[1] * v2[0];

	vecCopy3(tmp, vout);
}

void vecRotate(const vec3 v, const mat4 m, vec3 vout)
{
	vec3 tmp;

	tmp[0] =
		v[0] * m[0] +
		v[1] * m[4] +
		v[2] * m[8];

	tmp[1] =
		v[0] * m[1] +
		v[1] * m[5] +
		v[2] * m[9];

	tmp[2] =
		v[0] * m[2] +
		v[1] * m[6] +
		v[2] * m[10];

	vecCopy3(tmp, vout);
}

void vecInvRotate(const vec3 v, const mat4 m, vec3 vout)
{
	vec3 tmp;

	tmp[0] =
		v[0] * m[0] +
		v[1] * m[1] +
		v[2] * m[2];

	tmp[1] =
		v[0] * m[4] +
		v[1] * m[5] +
		v[2] * m[6];

	tmp[2] =
		v[0] * m[8] +
		v[1] * m[9] +
		v[2] * m[10];

	vecCopy3(tmp, vout);
}

void vecTransform(const vec3 v, const mat4 m, vec3 vout)
{
	vec3 tmp;

	tmp[0] =
		v[0] * m[0] +
		v[1] * m[4] +
		v[2] * m[8] +
		m[12];

	tmp[1] =
		v[0] * m[1] +
		v[1] * m[5] +
		v[2] * m[9] +
		m[13];

	tmp[2] =
		v[0] * m[2] +
		v[1] * m[6] +
		v[2] * m[10] +
		m[14];

	vecCopy3(tmp, vout);
}

void vecInvTransform(const vec3 v, const mat4 m, vec3 vout)
{
	vec3 tmp, tmp2;

	tmp[0] = v[0] - m[12];
	tmp[1] = v[1] - m[13];
	tmp[2] = v[2] - m[14];

	tmp2[0] =
		tmp[0] * m[0] +
		tmp[1] * m[1] +
		tmp[2] * m[2];

	tmp2[1] =
		tmp[0] * m[4] +
		tmp[1] * m[5] +
		tmp[2] * m[6];

	tmp2[2] =
		tmp[0] * m[8] +
		tmp[1] * m[9] +
		tmp[2] * m[10];

	vecCopy3(tmp2, vout);
}

void vecHermite(const vec3 p, const vec3 q, float t, vec3 h)
{
	float t2, t3, h1, h2;
	t2 = t * t;
	t3 = t2 * t;

	// 计算艾米基
	h1 =  2.0f * t3 - 3.0f * t2 + 1.0f;
	h2 = -2.0f * t3 + 3.0f * t2;

	// 进行艾米插值
	h[0] = h1 * p[0] + h2 * q[0];
	h[1] = h1 * p[1] + h2 * q[1];
	h[2] = h1 * p[2] + h2 * q[2];
}

void matIdentity(mat4 m)
{
	m[1] = m[2] = m[3] = m[4] = m[6] = m[7] = m[8] = m[9] = m[11] = m[12] = m[13] = m[14] = 0.0f;
	m[0] = m[5] = m[10] = m[15] = 1.0f;
}

// 生成一个 右手坐标系 透视投影 4x4 行矩阵
void matPerspectiveLHR(float fieldOfView,
					   float aspectRatio,
					   float zNear, float zFar,
					   mat4 m)
{
	float sine, cotangent, deltaZ;

	fieldOfView *= 0.5f;
	deltaZ = zFar - zNear;
	sine = sinf(fieldOfView);

	if(deltaZ != 0.0f && aspectRatio != 0.0f && sine != 0.0f)
	{
		cotangent = cosf(fieldOfView) / sine;

		// 第一行
		m[0] = cotangent / aspectRatio;
		m[1] = 0.0f;
		m[2] = 0.0f;
		m[3] = 0.0f;

		// 第二行
		m[4] = 0.0f;
		m[5] = cotangent;
		m[6] = 0.0f;
		m[7] = 0.0f;

		// 第三行
		m[8] = 0.0f;
		m[9] = 0.0f;
		m[10] = (zFar + zNear) / deltaZ;
		m[11] = 1.0f;

		// 第四行
		m[12] = 0.0f;
		m[13] = 0.0f;
		m[14] = -zNear * zFar / deltaZ;
		m[15] = 0.0f;
	}
}

// 生成一个 左手坐标系 透视投影 4x4 行矩阵
void matPerspectiveRHR(float fieldOfView,
					   float aspectRatio,
					   float zNear, float zFar,
					   mat4 m)
{
	matPerspectiveLHR(fieldOfView, aspectRatio, zNear, zFar, m);
	m[10] = -m[10];
	m[11] = -m[11];
}

// 生成一个 右手坐标系 透视投影 4x4 行矩阵
void matPerspectiveLHD(float fieldOfView,
					   float aspectRatio,
					   float zNear, float zFar,
					   mat4 m)
{
	matPerspectiveLHR(fieldOfView * to_radian, aspectRatio, zNear, zFar, m);
}

// 生成一个 左手坐标系 透视投影 4x4 行矩阵
void matPerspectiveRHD(float fieldOfView,
					   float aspectRatio,
					   float zNear, float zFar,
					   mat4 m)
{
	matPerspectiveLHR(fieldOfView * to_radian, aspectRatio, zNear, zFar, m);
	m[10] = -m[10];
	m[11] = -m[11];
}

void matLookAtLH(const vec3 eye, const vec3 at, const vec3 up, mat4 m)
{
	float x[3], y[3], z[3], mag;

	/* Difference eye and center vectors to make Z vector. */
	z[0] = at[0] - eye[0];
	z[1] = at[1] - eye[1];
	z[2] = at[2] - eye[2];
	/* Normalize Z. */
	mag = 1.0f / sqrtf(z[0] * z[0] + z[1] * z[1] + z[2] * z[2]);

	z[0] *= mag;
	z[1] *= mag;
	z[2] *= mag;

	/* Up vector makes Y vector. */
	y[0] = up[0];
	y[1] = up[1];
	y[2] = up[2];

	/* X vector = Y cross Z. */
	x[0] =  y[1] * z[2] - y[2] * z[1];
	x[1] = -y[0] * z[2] + y[2] * z[0];
	x[2] =  y[0] * z[1] - y[1] * z[0];

	/* Recompute Y = Z cross X. */
	y[0] =  z[1] * x[2] - z[2] * x[1];
	y[1] = -z[0] * x[2] + z[2] * x[0];
	y[2] =  z[0] * x[1] - z[1] * x[0];

	/* Normalize X. */
	mag = 1.0f / sqrtf(x[0] * x[0] + x[1] * x[1] + x[2] * x[2]);

	x[0] *= mag;
	x[1] *= mag;
	x[2] *= mag;

	/* Normalize Y. */
	mag = 1.0f / sqrtf(y[0] * y[0] + y[1] * y[1] + y[2] * y[2]);

	y[0] *= mag;
	y[1] *= mag;
	y[2] *= mag;

	m[0] = x[0];	m[4] = x[1];	m[8]  = x[2];	m[12] = -x[0] * eye[0] - x[1] * eye[1] - x[2] * eye[2];
	m[1] = y[0];	m[5] = y[1];	m[9]  = y[2];	m[13] = -y[0] * eye[0] - y[1] * eye[1] - y[2] * eye[2];
	m[2] = z[0];	m[6] = z[1];	m[10] = z[2];	m[14] = -z[0] * eye[0] - z[1] * eye[1] - z[2] * eye[2];
	m[3] = 0.0f;	m[7] = 0.0f;	m[11] = 0.0f;	m[15] = 1.0f;
}

void matLookAtRH(const vec3 eye, const vec3 at, const vec3 up, mat4 m)
{
	matLookAtLH(eye, at, up, m);
	m[0] = -m[0];
	m[2] = -m[2];
	m[4] = -m[4];
	m[6] = -m[6];
	m[8] = -m[8];
	m[10] = -m[10];
	m[12] = -m[12];
	m[14] = -m[14];
}

void matMulProj(const mat4 src1, const mat4 src2, mat4 dst)
{
	mat4 tmp;

	tmp[0] =
		src1[0] * src2[0] +
		src1[1] * src2[4] +
		src1[2] * src2[8] +
		src1[3] * src2[12];

	tmp[1] =
		src1[0] * src2[1] +
		src1[1] * src2[5] +
		src1[2] * src2[9] +
		src1[3] * src2[13];

	tmp[2] =
		src1[0] * src2[2] +
		src1[1] * src2[6] +
		src1[2] * src2[10] +
		src1[3] * src2[14];

	tmp[3] =
		src1[0] * src2[3] +
		src1[1] * src2[7] +
		src1[2] * src2[11] +
		src1[3] * src2[15];

	tmp[4] =
		src1[4] * src2[0] +
		src1[5] * src2[4] +
		src1[6] * src2[8] +
		src1[7] * src2[12];

	tmp[5] =
		src1[4] * src2[1] +
		src1[5] * src2[5] +
		src1[6] * src2[9] +
		src1[7] * src2[13];

	tmp[6] =
		src1[4] * src2[2] +
		src1[5] * src2[6] +
		src1[6] * src2[10] +
		src1[7] * src2[14];

	tmp[7] =
		src1[4] * src2[3] +
		src1[5] * src2[7] +
		src1[6] * src2[11] +
		src1[7] * src2[15];

	tmp[8] =
		src1[8] * src2[0] +
		src1[9] * src2[4] +
		src1[10] * src2[8] +
		src1[11] * src2[12];

	tmp[9] =
		src1[8] * src2[1] +
		src1[9] * src2[5] +
		src1[10] * src2[9] +
		src1[11] * src2[13];

	tmp[10] =
		src1[8] * src2[2] +
		src1[9] * src2[6] +
		src1[10] * src2[10] +
		src1[11] * src2[14];

	tmp[11] =
		src1[8] * src2[3] +
		src1[9] * src2[7] +
		src1[10] * src2[11] +
		src1[11] * src2[15];

	tmp[12] =
		src1[12] * src2[0] +
		src1[13] * src2[4] +
		src1[14] * src2[8] +
		src1[15] * src2[12];

	tmp[13] =
		src1[12] * src2[1] +
		src1[13] * src2[5] +
		src1[14] * src2[9] +
		src1[15] * src2[13];

	tmp[14] =
		src1[12] * src2[2] +
		src1[13] * src2[6] +
		src1[14] * src2[10] +
		src1[15] * src2[14];

	tmp[15] =
		src1[12] * src2[3] +
		src1[13] * src2[7] +
		src1[14] * src2[11] +
		src1[15] * src2[15];

	dst[0] = tmp[0];
	dst[1] = tmp[1];
	dst[2] = tmp[2];
	dst[3] = tmp[3];
	dst[4] = tmp[4];
	dst[5] = tmp[5];
	dst[6] = tmp[6];
	dst[7] = tmp[7];
	dst[8] = tmp[8];
	dst[9] = tmp[9];
	dst[10] = tmp[10];
	dst[11] = tmp[11];
	dst[12] = tmp[12];
	dst[13] = tmp[13];
	dst[14] = tmp[14];
	dst[15] = tmp[15];
}

void matMul(const mat4 src1, const mat4 src2, mat4 dst)
{
	mat4 tmp;

	tmp[0] =
		src1[0] * src2[0] +
		src1[1] * src2[4] +
		src1[2] * src2[8];

	tmp[1] =
		src1[0] * src2[1] +
		src1[1] * src2[5] +
		src1[2] * src2[9];

	tmp[2] =
		src1[0] * src2[2] +
		src1[1] * src2[6] +
		src1[2] * src2[10] +
		src1[3] * src2[14];


	tmp[4] =
		src1[4] * src2[0] +
		src1[5] * src2[4] +
		src1[6] * src2[8];

	tmp[5] =
		src1[4] * src2[1] +
		src1[5] * src2[5] +
		src1[6] * src2[9];

	tmp[6] =
		src1[4] * src2[2] +
		src1[5] * src2[6] +
		src1[6] * src2[10];


	tmp[8] =
		src1[8] * src2[0] +
		src1[9] * src2[4] +
		src1[10] * src2[8];

	tmp[9] =
		src1[8] * src2[1] +
		src1[9] * src2[5] +
		src1[10] * src2[9];

	tmp[10] =
		src1[8] * src2[2] +
		src1[9] * src2[6] +
		src1[10] * src2[10];


	tmp[12] =
		src1[12] * src2[0] +
		src1[13] * src2[4] +
		src1[14] * src2[8] +
		src2[12];

	tmp[13] =
		src1[12] * src2[1] +
		src1[13] * src2[5] +
		src1[14] * src2[9] +
		src2[13];

	tmp[14] =
		src1[12] * src2[2] +
		src1[13] * src2[6] +
		src1[14] * src2[10] +
		src2[14];


	dst[0] = tmp[0];
	dst[1] = tmp[1];
	dst[2] = tmp[2];
	dst[3] = 0.0f;
	dst[4] = tmp[4];
	dst[5] = tmp[5];
	dst[6] = tmp[6];
	dst[7] = 0.0f;
	dst[8] = tmp[8];
	dst[9] = tmp[9];
	dst[10] = tmp[10];
	dst[11] = 0.0f;
	dst[12] = tmp[12];
	dst[13] = tmp[13];
	dst[14] = tmp[14];
	dst[15] = 1.0f;
}

void matAngleR(const vec3 radians, mat4 m)
{
	float rad;
	float sr, sp, sy, cr, cp, cy;

	rad = radians[2];
	sy = sinf(rad);
	cy = cosf(rad);
	rad = radians[1];
	sp = sinf(rad);
	cp = cosf(rad);
	rad = radians[0];
	sr = sinf(rad);
	cr = cosf(rad);

	// matrix = (Z * Y) * X
	m[0]  = cp * cy;
	m[1]  = cp * sy;
	m[2]  = -sp;
	m[3]  = 0.0f;
	m[4]  = sr * sp * cy + cr * -sy;
	m[5]  = sr * sp * sy + cr * cy;
	m[6]  = sr * cp;
	m[7]  = 0.0f;
	m[8]  = cr * sp * cy + -sr * -sy;
	m[9]  = cr * sp * sy + -sr * cy;
	m[10] = cr * cp;
	m[11] = 0.0f;
	m[12] = 0.0f;
	m[13] = 0.0f;
	m[14] = 0.0f;
	m[15] = 1.0f;
}

void matRotateR2(float radian, const vec3 axis, mat4 m)
{
	float sine, cosine, ab, bc, ca, tx, ty, tz;

	sine = sinf(radian);
	cosine = cosf(radian);
	ab = axis[0] * axis[1] * (1.0f - cosine);
	bc = axis[1] * axis[2] * (1.0f - cosine);
	ca = axis[2] * axis[0] * (1.0f - cosine);
	tx = axis[0] * axis[0];
	ty = axis[1] * axis[1];
	tz = axis[2] * axis[2];

	m[0]  = tx + cosine * (1.0f - tx);
	m[1]  = ab + axis[2] * sine;
	m[2]  = ca - axis[1] * sine;
	m[3]  = 0.0f;
	m[4]  = ab - axis[2] * sine;
	m[5]  = ty + cosine * (1.0f - ty);
	m[6]  = bc + axis[0] * sine;
	m[7]  = 0.0f;
	m[8]  = ca + axis[1] * sine;
	m[9]  = bc - axis[0] * sine;
	m[10] = tz + cosine * (1.0f - tz);
	m[11] = 0.0f;
	m[12] = 0.0f;
	m[13] = 0.0f;
	m[14] = 0.0f;
	m[15] = 1.0f;
}

void matRotateR(float radian, float ax, float ay, float az, mat4 m)
{
	vec3 axis = { ax, ay, az };
	matRotateR2(radian, axis, m);
}

void matRotateD2(float degree, const vec3 axis, mat4 m)
{
	matRotateR2(degree * to_radian, axis, m);
}

void matRotateD(float degree, float ax, float ay, float az, mat4 m)
{
	vec3 axis = { ax, ay, az };
	matRotateR2(degree * to_radian, axis, m);
}

void matTranslate2(const vec3 axis, mat4 m)
{
	m[1] = m[2] = m[3] = m[4] = m[6] = m[7] = m[8] = m[9] = m[11] = 0.0f;

	m[0]  = 1.0f;
	m[5]  = 1.0f;
	m[10] = 1.0f;
	m[12] = axis[0];
	m[13] = axis[1];
	m[14] = axis[2];
	m[15] = 1.0f;
}

void matTranslate(float x, float y, float z, mat4 m)
{
	m[1] = m[2] = m[3] = m[4] = m[6] = m[7] = m[8] = m[9] = m[11] = 0.0f;

	m[0]  = 1.0f;
	m[5]  = 1.0f;
	m[10] = 1.0f;
	m[12] = x;
	m[13] = y;
	m[14] = z;
	m[15] = 1.0f;
}

void matScale2(const vec3 axis, mat4 m)
{
	m[1] = m[2] = m[3] = m[4] = m[6] = m[7] = m[8] = m[9] = m[11] = m[12] = m[13] = m[14] = 0.0f;

	m[0]  = axis[0];
	m[5]  = axis[1];
	m[10] = axis[2];
	m[15] = 1.0f;
}

void matScale(float x, float y, float z, mat4 m)
{
	m[1] = m[2] = m[3] = m[4] = m[6] = m[7] = m[8] = m[9] = m[11] = m[12] = m[13] = m[14] = 0.0f;

	m[0]  = x;
	m[5]  = y;
	m[10] = z;
	m[15] = 1.0f;
}

void matInverse(const mat4 mat, mat4 m)
{
	mat4 tmp;

	tmp[ 0] = mat[ 5]*mat[10] - mat[ 6]*mat[ 9];
	tmp[ 1] = mat[ 2]*mat[ 9] - mat[ 1]*mat[10];
	tmp[ 2] = mat[ 1]*mat[ 6] - mat[ 2]*mat[ 5];

	tmp[ 4] = mat[ 6]*mat[ 8] - mat[ 4]*mat[10];
	tmp[ 5] = mat[ 0]*mat[10] - mat[ 2]*mat[ 8];
	tmp[ 6] = mat[ 2]*mat[ 4] - mat[ 0]*mat[ 6];

	tmp[ 8] = mat[ 4]*mat[ 9] - mat[ 5]*mat[ 8];
	tmp[ 9] = mat[ 1]*mat[ 8] - mat[ 0]*mat[ 9];
	tmp[10] = mat[ 0]*mat[ 5] - mat[ 1]*mat[ 4];

	tmp[12] = mat[ 4]*mat[10]*mat[13] + mat[ 5]*mat[ 8]*mat[14] + mat[ 6]*mat[ 9]*mat[12] - mat[ 4]*mat[ 9]*mat[14] - mat[ 5]*mat[10]*mat[12] - mat[ 6]*mat[ 8]*mat[13];
	tmp[13] = mat[ 0]*mat[ 9]*mat[14] + mat[ 1]*mat[10]*mat[12] + mat[ 2]*mat[ 8]*mat[13] - mat[ 0]*mat[10]*mat[13] - mat[ 1]*mat[ 8]*mat[14] - mat[ 2]*mat[ 9]*mat[12];
	tmp[14] = mat[ 0]*mat[ 6]*mat[13] + mat[ 1]*mat[ 4]*mat[14] + mat[ 2]*mat[ 5]*mat[12] - mat[ 0]*mat[ 5]*mat[14] - mat[ 1]*mat[ 6]*mat[12] - mat[ 2]*mat[ 4]*mat[13];

	m[0] = tmp[0];
	m[1] = tmp[1];
	m[2] = tmp[2];
	m[3] = 0.0f;
	m[4] = tmp[4];
	m[5] = tmp[5];
	m[6] = tmp[6];
	m[7] = 0.0f;
	m[8] = tmp[8];
	m[9] = tmp[9];
	m[10] = tmp[10];
	m[11] = 0.0f;
	m[12] = tmp[12];
	m[13] = tmp[13];
	m[14] = tmp[14];
	m[15] = 1.0f;
}

void matQuatTrans(const quat quaternion, const vec3 trans, mat4 matrix)
{
	matQuat(quaternion, matrix);
	matrix[12] = trans[0];
	matrix[13] = trans[1];
	matrix[14] = trans[2];
}

void matQuat(const quat quaternion, mat4 m)
{
	m[0]  = 1.0f - 2.0f * quaternion[1] * quaternion[1] - 2.0f * quaternion[2] * quaternion[2];
	m[1]  = 2.0f * quaternion[0] * quaternion[1] + 2.0f * quaternion[3] * quaternion[2];
	m[2]  = 2.0f * quaternion[0] * quaternion[2] - 2.0f * quaternion[3] * quaternion[1];
	m[3]  = 0.0f;

	m[4]  = 2.0f * quaternion[0] * quaternion[1] - 2.0f * quaternion[3] * quaternion[2];
	m[5]  = 1.0f - 2.0f * quaternion[0] * quaternion[0] - 2.0f * quaternion[2] * quaternion[2];
	m[6]  = 2.0f * quaternion[1] * quaternion[2] + 2.0f * quaternion[3] * quaternion[0];
	m[7]  = 0.0f;

	m[8]  = 2.0f * quaternion[0] * quaternion[2] + 2.0f * quaternion[3] * quaternion[1];
	m[9]  = 2.0f * quaternion[1] * quaternion[2] - 2.0f * quaternion[3] * quaternion[0];
	m[10] = 1.0f - 2.0f * quaternion[0] * quaternion[0] - 2.0f * quaternion[1] * quaternion[1];
	m[11] = 0.0f;

	m[12] = 0.0f;
	m[13] = 0.0f;
	m[14] = 0.0f;
	m[15] = 1.0f;
}

void quaMatrix(const mat4 mat, quat quaternion)
{
	float w4;
	quaternion[3] = sqrtf(1.0f + mat[0] + mat[5] + mat[10]) * 0.5f;
	w4 = 1.0f / (4.0f * quaternion[3]);
	quaternion[0] = (mat[6] - mat[9]) * w4;
	quaternion[1] = (mat[8] - mat[2]) * w4;
	quaternion[2] = (mat[1] - mat[4]) * w4;
}

void quaAngleR(const vec3 radians, quat quaternion)
{
	float angle;
	float sr, sp, sy, cr, cp, cy;

	// FIXME: rescale the inputs to 1/2 angle
	angle = radians[2] * 0.5f;
	sy = sinf(angle);
	cy = cosf(angle);
	angle = radians[1] * 0.5f;
	sp = sinf(angle);
	cp = cosf(angle);
	angle = radians[0] * 0.5f;
	sr = sinf(angle);
	cr = cosf(angle);

	quaternion[0] = sr * cp * cy - cr * sp * sy; // X
	quaternion[1] = cr * sp * cy + sr * cp * sy; // Y
	quaternion[2] = cr * cp * sy - sr * sp * cy; // Z
	quaternion[3] = cr * cp * cy + sr * sp * sy; // W
}

void quaSlerp(const quat p, const quat q, float t, quat quaternion)
{
	uchar i;
	float omega, cosom, sinom, sclp, sclq;
	quat tmpq = { q[0], q[1], q[2], q[3] };

	// decide if one of the quaternions is backwards
	float a = 0.0f;
	float b = 0.0f;
	for(i = 0; i < 4; i++)
	{
		a += (p[i] - tmpq[i]) * (p[i] - tmpq[i]);
		b += (p[i] + tmpq[i]) * (p[i] + tmpq[i]);
	}
	if(a > b)
	{
		tmpq[0] = -tmpq[0];
		tmpq[1] = -tmpq[1];
		tmpq[2] = -tmpq[2];
		tmpq[3] = -tmpq[3];
	}

	cosom = p[0] * tmpq[0] + p[1] * tmpq[1] + p[2] * tmpq[2] + p[3] * tmpq[3];

	if((1.0f + cosom) > 0.00000001f)
	{
		if((1.0f - cosom) > 0.00000001f)
		{
			omega = acosf(cosom);
			sinom = sinf(omega);
			sclp = sinf((1.0f - t) * omega) / sinom;
			sclq = sinf(t * omega) / sinom;
		}
		else
		{
			sclp = 1.0f - t;
			sclq = t;
		}
		for(i = 0; i < 4; i++)
		{
			quaternion[i] = sclp * p[i] + sclq * tmpq[i];
		}
	}
	else
	{
		quaternion[0] = -p[1];
		quaternion[1] = p[0];
		quaternion[2] = -p[3];
		quaternion[3] = p[2];
		sclp = sinf((1.0f - t) * 0.5f * PI);
		sclq = sinf(t * 0.5f * PI);

		quaternion[0] = sclp * p[0] + sclq * quaternion[0];
		quaternion[1] = sclp * p[1] + sclq * quaternion[1];
		quaternion[2] = sclp * p[2] + sclq * quaternion[2];
	}
}

// 计算切线空间的向量 (源自 Ogre)
void tangentSpaceLH(const vec3 p1, const vec3 p2, const vec3 p3,
					const vec2 texc1, const vec2 texc2, const vec2 texc3,
					vec3 out_normal, vec3 out_tangent, vec3 out_binormal)
{
	vec3 tmp1, tmp2;
	vec3 side0, side1, normal, tangent, binormal;
	vec3 tangentCross;
	float deltaV0, deltaV1, deltaU0, deltaU1;

	// side0 = p2 - p1;
	vecSub3(p2, p1, side0);

	// side1 = p3 - p1;
	vecSub3(p3, p1, side1);

	// normal = cross(side1, side0)
	vecCross(side1, side0, normal);
	vecNormalize(normal, normal);

	// deltaV0 = texc2.v - texc1.v
	deltaV0 = texc2[1] - texc1[1];
	// deltaV1 = texc3.v - texc1.v
	deltaV1 = texc3[1] - texc1[1];

	// tmp1 = deltaV1 * side0;
	vecMulNum3(side0, deltaV1, tmp1);

	// tmp2 = deltaV0 * side1;
	vecMulNum3(side1, deltaV0, tmp2);

	// tangent = deltaV1 * side0 - deltaV0 * side1;
	vecSub3(tmp1, tmp2, tangent);

	vecNormalize(tangent, tangent);


	// deltaU0 = texc2.u - texc1.u
	deltaU0 = texc2[0] - texc1[0];
	// deltaU1 = texc3.u - texc1.u
	deltaU1 = texc3[0] - texc1[0];

	vecMulNum3(side0, deltaU1, tmp1);

	vecMulNum3(side1, deltaU0, tmp2);

	// binormal = deltaU1 * side0 - deltaU0 * side1;
	vecSub3(tmp1, tmp2, binormal);

	vecNormalize(binormal, binormal);

	// 求法线
	// tangentCross = cross(tangent, binormal)
	vecCross(tangent, binormal, tangentCross);

	// 与原法线比较, 是否反了
	if(vecDot3(tangentCross, normal) < 0.0f)
	{
		tangent[0] = -tangent[0];
		tangent[1] = -tangent[1];
		tangent[2] = -tangent[2];

		binormal[0] = -binormal[0];
		binormal[1] = -binormal[1];
		binormal[2] = -binormal[2];
	}

	if(out_normal) vecCopy3(normal, out_normal);
	if(out_tangent) vecCopy3(tangent, out_tangent);
	if(out_binormal) vecCopy3(binormal, out_binormal);
}

void camFirstPersonD(float *ax, float *ay, const vec3 up, vec3 cross, vec3 vout)
{
	vec4 vec = { 0.0f, 0.0f, 1.0f, 1.0f };
	mat4 mat1, mat2;

	// 仰角限制
	if(*ay > 89.9f) *ay = 89.9f;
	else if(*ay < -89.9f) *ay = -89.9f;
	// 角度夹取
	if(*ax > 360.0f) *ax -= 360.0f;
	else if(*ax < 0.0f) *ax += 360.0f;

	vecCross(up, vec, cross);

	matRotateD2(*ay, cross, mat1);
	matRotateD2(*ax, up, mat2);
	matMul(mat1, mat2, mat2);
	vecTransform(vec, mat2, vec);

	vecCross(up, vec, cross);

	vout[0] = vec[0];
	vout[1] = vec[1];
	vout[2] = vec[2];
}

