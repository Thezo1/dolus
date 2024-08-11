// NOTE: Never forget to clamp packing colors, saves you a whole lot of headache

#ifndef _H_DOLUSMATH
#define _H_DOLUSMATH

#include<stdint.h>
#include<stdbool.h>
#include<float.h>
#include <xmmintrin.h>

#define F32MAX FLT_MAX
#define F32MIN -FLT_MAX
#define PI32 3.14159265358979f
#define EPSILON 0.001f

typedef float f32;
typedef double f64;

typedef uint32_t u32;
typedef uint8_t u8;

typedef struct
{
    f32 x, y;
}v2;

extern inline f32 FRAND()
{
    f32 result = (f32)rand()/((f32)RAND_MAX+1.0f);
    return(result);
}

extern inline f64 DRAND()
{
    f64 result = rand()/(RAND_MAX+1.0);
    return(result);
}

extern inline f32 f32_random_within(f32 min, f32 max)
{
    return min + (max-min)*FRAND();
}

extern inline f32 f64_random_within(f32 min, f32 max)
{
    return min + (max-min)*DRAND();
}

extern inline v2 V2(f32 A, f32 B)
{
    v2 result;
    result.x = A;
    result.y = B;
    return(result);
}

extern inline v2 v2_add(v2 A, v2 B)
{
    v2 result;
    result.x = A.x + B.x;
    result.y = A.y + B.y;
    return(result);
}

extern inline v2 v2_sub(v2 A, v2 B)
{
    v2 result;
    result.x = A.x - B.x;
    result.y = A.y - B.y;
    return(result);
}

extern inline v2 v2_neg(v2 A)
{
    v2 result;
    result.x = -A.x;
    result.y = -A.y;
    return(result);
}

extern inline v2 v2_scalar_mul(v2 A, f32 S)
{
    v2 result;
    result.x = A.x * S;
    result.y = A.y * S;

    return(result);
}

typedef struct
{
    f32 x, y, z;
}v3;

extern inline v3 V3(f32 A, f32 B, f32 C)
{
    v3 result;
    result.x = A;
    result.y = B;
    result.z = C;
    return(result);
}

extern inline v3 v3_add(v3 A, v3 B)
{
    v3 result;
    result.x = A.x + B.x;
    result.y = A.y + B.y;
    result.z = A.z + B.z;
    return(result);
}

extern inline v3 v3_sub(v3 A, v3 B)
{
    v3 result;
    result.x = A.x - B.x;
    result.y = A.y - B.y;
    result.z = A.z - B.z;
    return(result);
}

extern inline v3 v3_mul(v3 A, v3 B)
{
    v3 result;
    result.x = A.x * B.x;
    result.y = A.y * B.y;
    result.z = A.z * B.z;
    return(result);
}

extern inline v3 v3_neg(v3 A)
{
    v3 result;
    result.x = -A.x;
    result.y = -A.y;
    result.z = -A.z;
    return(result);
}

extern inline v3 v3_scalar_mul(v3 A, f32 S)
{
    v3 result;
    result.x = A.x * S;
    result.y = A.y * S;
    result.z = A.z * S;

    return(result);
}

extern inline v3 v3_scalar_div(v3 A, f32 S)
{
    v3 result;
    f32 inv = 1/S;
    result.x = A.x * inv;
    result.y = A.y * inv;
    result.z = A.z * inv;

    return(result);
}

extern inline v3 v3_scalar_add(v3 A, f32 S)
{
    v3 result;
    result.x = A.x + S;
    result.y = A.y + S;
    result.z = A.z + S;

    return(result);
}

extern inline f32 dot(v3 A, v3 B)
{
    f32 result = A.x * B.x + A.y * B.y + A.z * B.z;
    return(result);
}

extern inline v3 cross(v3 A, v3 B)
{
    v3 result;

    result.x = A.y * B.z - A.z * B.y;
    result.y = A.z * B.x - A.x * B.z;
    result.z = A.x * B.y - A.y * B.x;

    return(result);
}

extern inline f32 square(f32 F32)
{
    f32 result = F32 * F32;
    return(result);
}

extern inline f32 square_root(f32 F32)
{
    f32 result = _mm_cvtss_f32(_mm_sqrt_ss(_mm_set_ss(F32)));
    return(result);
}

extern inline f32 length_sq(v3 A)
{
    f32 result = dot(A, A);
    return(result);
}

extern inline f32 length(v3 A)
{
    f32 result = square_root(length_sq(A));
    return(result);
}

extern inline v3 normalize(v3 A)
{
    v3 result;
    v3_scalar_mul(A, (1/length(A)));
    return(result);
}

extern inline v3 NOZ(v3 A)
{
    v3 result = {};
    f32 len_sq = length_sq(A);
    if(len_sq > square(0.0001f))
    {
        result = v3_scalar_mul(A, (1.0f/square_root(len_sq)));
    }
    return(result);
}

extern inline v3 reflect(v3 I, v3 N) 
{
    v3 result = {};
    result = v3_sub(I, v3_scalar_mul(N, (2.0f * dot(I, N))));
    return(result);
}

extern inline v3 v3_random()
{
    return(V3(DRAND(), DRAND(), DRAND()));
}

extern inline v3 v3_random_within(f32 min, f32 max)
{
    return(V3(f32_random_within(min, max), f32_random_within(min, max), f32_random_within(min, max)));
}

extern inline v3 random_in_unit_sphere()
{
    while(1)
    {
        v3 p = v3_random_within(-1.0f, 1.0f);
        if(length_sq(p) < 1)
        {
            return p;
        }
    }
}

extern inline v3 random_on_hemishere(v3 normal)
{
    v3 on_unit_sphere = NOZ(random_in_unit_sphere());
    if(dot(on_unit_sphere, normal) > 0.0f)
    {
        return(on_unit_sphere);
    }
    else
    {
        return(v3_neg(on_unit_sphere));
    }
}

typedef struct
{
    f32 x, y, z, w;
}v4;

extern inline v4 V4(f32 A, f32 B, f32 C, f32 D)
{
    v4 result = {};

    result.x = A;
    result.y = B;
    result.z = C;
    result.w = D;

    return(result);
}

extern inline v4 v4_add(v4 A, v4 B)
{
    v4 result = {};
    result.x = A.x + B.x;
    result.y = A.y + B.y;
    result.z = A.z + B.z;
    result.w = A.w + B.w;
    return(result);
}

extern inline v4 v4_sub(v4 A, v4 B)
{
    v4 result = {};
    result.x = A.x - B.x;
    result.y = A.y - B.y;
    result.z = A.z - B.z;
    result.w = A.w - B.w;
    return(result);
}

extern inline v4 v4_mul(v4 A, v4 B)
{
    v4 result;
    result.x = A.x * B.x;
    result.y = A.y * B.y;
    result.z = A.z * B.z;
    result.w = A.w * B.w;
    return(result);
}

extern inline v4 v4_neg(v4 A)
{
    v4 result = {};
    result.x = -A.x;
    result.y = -A.y;
    result.z = -A.z;
    result.w = -A.w;
    return(result);
}

extern inline v4 v4_scalar_mul(v4 A, f32 S)
{
    v4 result = {};
    result.x = A.x * S;
    result.y = A.y * S;
    result.z = A.z * S;
    result.w = A.w * S;

    return(result);
}

extern inline v4 v4_scalar_div(v4 A, f32 S)
{
    v4 result = {};
    f32 inv = 1/S;
    result.x = A.x * inv;
    result.y = A.y * inv;
    result.z = A.z * inv;
    result.w = A.w * inv;

    return(result);
}

extern inline v4 v4_scalar_add(v4 A, f32 S)
{
    v4 result = {};
    result.x = A.x + S;
    result.y = A.y + S;
    result.z = A.z + S;
    result.w = A.w + S;

    return(result);
}

extern inline f32 v4_dot(v4 A, v4 B)
{
    f32 result = A.x * B.x + A.y * B.y + A.z * B.z + A.w * B.w;
    return(result);
}

extern inline f32 v4_length(v4 A)
{
    f32 result = square_root(v4_dot(A, A));
    return(result);
}

extern inline f32 v4_length_sq(v4 A)
{
    f32 result = v4_dot(A, A);
    return(result);
}

extern inline v4 v4_normalize(v4 A)
{
    v4 result = {};
    f32 len_sq = v4_length_sq(A);
    if(len_sq > square(0.0001f))
    {
        result = v4_scalar_mul(A, (1.0f/square_root(len_sq)));
    }
    return(result);
}

extern inline v4 v4_cross(v4 A, v4 B)
{
    v4 result = {};

    result.x = A.y * B.z - A.z * B.y;
    result.y = A.z * B.x - A.x * B.z;
    result.z = A.x * B.y - A.y * B.x;
    result.w = 0.0f;

    return(result);
}

extern inline v3 v4_v3(v4 A)
{
    v3 result = {};
    result.x = A.x;
    result.y = A.y;
    result.z = A.z;
    return(result);
}

extern inline v4 Point(f32 A, f32 B, f32 C)
{
    v4 result = V4(A, B, C, 1);
    return(result);
}

extern inline v4 Vector(f32 A, f32 B, f32 C)
{
    v4 result = V4(A, B, C, 0);
    return(result);
}

#include<math.h>
extern inline f32 POW(f32 B, f32 P)
{
    f32 result = (f32)pow(B, P);

    return(result);
}

extern inline f32 TAN(f32 R)
{
    return((f32)tan(R));
}

extern inline f32 clamp(f32 num, f32 min, f32 max)
{
    if(num > max)
    {
        return(max);
    }

    if(num < min)
    {
        return(min);
    }
    return(num);
}

// NOTE: Don't really know if the disparity between big and little endian
// id Correct
extern inline u32 pack_color_little(v3 color)
{
    u32 result;
    u32 r = (u32)(clamp(color.x, 0.0f, 1.0f) * 255);
    u32 g = (u32)(clamp(color.y, 0.0f, 1.0f) * 255);
    u32 b = (u32)(clamp(color.z, 0.0f, 1.0f) * 255);

    result = (b << 8*0) | 
             (g << 8*1) | 
             (r << 8*2);

    return(result);
}

extern inline u32 pack_color_big(v3 color)
{
    u32 result;

    u32 r = (u32)(clamp(color.x, 0.0f, 1.0f) * 255);
    u32 g = (u32)(clamp(color.y, 0.0f, 1.0f) * 255);
    u32 b = (u32)(clamp(color.z, 0.0f, 1.0f) * 255);

    result = (b << 8*2) | 
             (g << 8*1) | 
             (r << 8*0);

    return(result);
}

typedef struct m4x4
{
    v4 rows[4];
}m4x4;

extern inline m4x4 m4x4_mul(m4x4 a, m4x4 b)
{
    m4x4 result = {};

	result.rows[0] = V4(
		a.rows[0].x * b.rows[0].x  +  a.rows[0].y * b.rows[1].x  +  a.rows[0].z * b.rows[2].x  +  a.rows[0].w * b.rows[3].x,
		a.rows[0].x * b.rows[0].y  +  a.rows[0].y * b.rows[1].y  +  a.rows[0].z * b.rows[2].y  +  a.rows[0].w * b.rows[3].y,
		a.rows[0].x * b.rows[0].z  +  a.rows[0].y * b.rows[1].z  +  a.rows[0].z * b.rows[2].z  +  a.rows[0].w * b.rows[3].z,
		a.rows[0].x * b.rows[0].w  +  a.rows[0].y * b.rows[1].w  +  a.rows[0].z * b.rows[2].w  +  a.rows[0].w * b.rows[3].w
	);

	result.rows[1] = V4(
		a.rows[1].x * b.rows[0].x  +  a.rows[1].y * b.rows[1].x  +  a.rows[1].z * b.rows[2].x  +  a.rows[1].w * b.rows[3].x,
		a.rows[1].x * b.rows[0].y  +  a.rows[1].y * b.rows[1].y  +  a.rows[1].z * b.rows[2].y  +  a.rows[1].w * b.rows[3].y,
		a.rows[1].x * b.rows[0].z  +  a.rows[1].y * b.rows[1].z  +  a.rows[1].z * b.rows[2].z  +  a.rows[1].w * b.rows[3].z,
		a.rows[1].x * b.rows[0].w  +  a.rows[1].y * b.rows[1].w  +  a.rows[1].z * b.rows[2].w  +  a.rows[1].w * b.rows[3].w
	);

	result.rows[2] = V4(
		a.rows[2].x * b.rows[0].x  +  a.rows[2].y * b.rows[1].x  +  a.rows[2].z * b.rows[2].x  +  a.rows[2].w * b.rows[3].x,
		a.rows[2].x * b.rows[0].y  +  a.rows[2].y * b.rows[1].y  +  a.rows[2].z * b.rows[2].y  +  a.rows[2].w * b.rows[3].y,
		a.rows[2].x * b.rows[0].z  +  a.rows[2].y * b.rows[1].z  +  a.rows[2].z * b.rows[2].z  +  a.rows[2].w * b.rows[3].z,
		a.rows[2].x * b.rows[0].w  +  a.rows[2].y * b.rows[1].w  +  a.rows[2].z * b.rows[2].w  +  a.rows[2].w * b.rows[3].w
	);

	result.rows[3] = V4(
		a.rows[3].x * b.rows[0].x  +  a.rows[3].y * b.rows[1].x  +  a.rows[3].z * b.rows[2].x  +  a.rows[3].w * b.rows[3].x,
		a.rows[3].x * b.rows[0].y  +  a.rows[3].y * b.rows[1].y  +  a.rows[3].z * b.rows[2].y  +  a.rows[3].w * b.rows[3].y,
		a.rows[3].x * b.rows[0].z  +  a.rows[3].y * b.rows[1].z  +  a.rows[3].z * b.rows[2].z  +  a.rows[3].w * b.rows[3].z,
		a.rows[3].x * b.rows[0].w  +  a.rows[3].y * b.rows[1].w  +  a.rows[3].z * b.rows[2].w  +  a.rows[3].w * b.rows[3].w
	);
    return(result);
}

extern inline m4x4 m4x4_transpose(m4x4 a)
{
    m4x4 result = {};
    
    result.rows[0] = V4(a.rows[0].x, a.rows[1].x, a.rows[2].x, a.rows[3].x);
    result.rows[1] = V4(a.rows[0].y, a.rows[1].y, a.rows[2].y, a.rows[3].y);
    result.rows[2] = V4(a.rows[0].z, a.rows[1].z, a.rows[2].z, a.rows[3].z);
    result.rows[3] = V4(a.rows[0].w, a.rows[1].w, a.rows[2].w, a.rows[3].w);

    return result;
}

extern inline m4x4 m4x4_identity() 
{

    m4x4 result = {};

	result.rows[0] = V4(1.0, 0.0, 0.0, 0.0);
	result.rows[1] = V4(0.0, 1.0, 0.0, 0.0);
	result.rows[2] = V4(0.0, 0.0, 1.0, 0.0);
	result.rows[3] = V4(0.0, 0.0, 0.0, 1.0);

    return(result);
}

extern inline bool m4x4_invert(m4x4 m, m4x4 *invOut)
{
    m4x4 inv;
    f32 det;
    int i;

    // 0
    inv.rows[0].x = m.rows[1].y  * m.rows[2].z  * m.rows[3].w - 
                    m.rows[1].y  * m.rows[2].w  * m.rows[3].z - 
                    m.rows[2].y  * m.rows[1].z  * m.rows[3].w + 
                    m.rows[2].y  * m.rows[1].w  * m.rows[3].z +
                    m.rows[3].y  * m.rows[1].z  * m.rows[2].w - 
                    m.rows[3].y  * m.rows[1].w  * m.rows[2].z;

    // 4
    inv.rows[1].x = -m.rows[1].x  * m.rows[2].z  * m.rows[3].w + 
                     m.rows[1].x  * m.rows[2].w  * m.rows[3].z + 
                     m.rows[2].x  * m.rows[1].z  * m.rows[3].w - 
                     m.rows[2].x  * m.rows[1].w  * m.rows[3].z - 
                     m.rows[3].x  * m.rows[1].z  * m.rows[2].w + 
                     m.rows[3].x  * m.rows[1].w  * m.rows[2].z;

    // 8
    inv.rows[2].x = m.rows[1].x  * m.rows[2].y * m.rows[3].w - 
                    m.rows[1].x  * m.rows[2].w * m.rows[3].y - 
                    m.rows[2].x  * m.rows[1].y * m.rows[3].w + 
                    m.rows[2].x  * m.rows[1].w * m.rows[3].y + 
                    m.rows[3].x  * m.rows[1].y * m.rows[2].w - 
                    m.rows[3].x  * m.rows[1].w * m.rows[2].y;

    // 12
    inv.rows[3].x = -m.rows[1].x  * m.rows[2].y  * m.rows[3].z + 
                     m.rows[1].x  * m.rows[2].z  * m.rows[3].y +
                     m.rows[2].x  * m.rows[1].y  * m.rows[3].z - 
                     m.rows[2].x  * m.rows[1].z  * m.rows[3].y - 
                     m.rows[3].x  * m.rows[1].y  * m.rows[2].z + 
                     m.rows[3].x  * m.rows[1].z  * m.rows[2].y;

    // 1
    inv.rows[0].y = -m.rows[0].y  * m.rows[2].z  * m.rows[3].w + 
                     m.rows[0].y  * m.rows[2].w  * m.rows[3].z + 
                     m.rows[2].y  * m.rows[0].z  * m.rows[3].w - 
                     m.rows[2].y  * m.rows[0].w  * m.rows[3].z - 
                     m.rows[3].y  * m.rows[0].z  * m.rows[2].w + 
                     m.rows[3].y  * m.rows[0].w  * m.rows[2].z;

    // 5
    inv.rows[1].y = m.rows[0].x  * m.rows[2].z  * m.rows[3].w - 
                    m.rows[0].x  * m.rows[2].w  * m.rows[3].z - 
                    m.rows[2].x  * m.rows[0].z  * m.rows[3].w + 
                    m.rows[2].x  * m.rows[0].w  * m.rows[3].z + 
                    m.rows[3].x  * m.rows[0].z  * m.rows[2].w - 
                    m.rows[3].x  * m.rows[0].w  * m.rows[2].z;

    // 9
    inv.rows[2].y = -m.rows[0].x  * m.rows[2].y    * m.rows[3].w + 
                     m.rows[0].x  * m.rows[2].w    * m.rows[3].y + 
                     m.rows[2].x  * m.rows[0].y    * m.rows[3].w - 
                     m.rows[2].x  * m.rows[0].w    * m.rows[3].y - 
                     m.rows[3].x  * m.rows[0].y    * m.rows[2].w + 
                     m.rows[3].x  * m.rows[0].w    * m.rows[2].y;

    // 13
    inv.rows[3].y = m.rows[0].x  * m.rows[2].y  * m.rows[3].z - 
                    m.rows[0].x  * m.rows[2].z  * m.rows[3].y - 
                    m.rows[2].x  * m.rows[0].y  * m.rows[3].z + 
                    m.rows[2].x  * m.rows[0].z  * m.rows[3].y + 
                    m.rows[3].x  * m.rows[0].y  * m.rows[2].z - 
                    m.rows[3].x  * m.rows[0].z  * m.rows[2].y;

    // 2
    inv.rows[0].z = m.rows[0].y  * m.rows[1].z * m.rows[3].w - 
                    m.rows[0].y  * m.rows[1].w * m.rows[3].z - 
                    m.rows[1].y  * m.rows[0].z * m.rows[3].w + 
                    m.rows[1].y  * m.rows[0].w * m.rows[3].z + 
                    m.rows[3].y  * m.rows[0].z * m.rows[1].w - 
                    m.rows[3].y  * m.rows[0].w * m.rows[1].z;

    // 6
    inv.rows[1].z = -m.rows[0].x  * m.rows[1].z * m.rows[3].w + 
                     m.rows[0].x  * m.rows[1].w * m.rows[3].z + 
                     m.rows[1].x  * m.rows[0].z * m.rows[3].w - 
                     m.rows[1].x  * m.rows[0].w * m.rows[3].z - 
                     m.rows[3].x  * m.rows[0].z * m.rows[1].w + 
                     m.rows[3].x  * m.rows[0].w * m.rows[1].z;

    // 10
    inv.rows[2].z = m.rows[0].x  * m.rows[1].y * m.rows[3].w - 
                    m.rows[0].x  * m.rows[1].w * m.rows[3].y - 
                    m.rows[1].x  * m.rows[0].y * m.rows[3].w + 
                    m.rows[1].x  * m.rows[0].w * m.rows[3].y + 
                    m.rows[3].x  * m.rows[0].y * m.rows[1].w - 
                    m.rows[3].x  * m.rows[0].w * m.rows[1].y;

    // 14
    inv.rows[3].z = -m.rows[0].x  * m.rows[1].y * m.rows[3].z + 
                     m.rows[0].x  * m.rows[1].z * m.rows[3].y + 
                     m.rows[1].x  * m.rows[0].y * m.rows[3].z - 
                     m.rows[1].x  * m.rows[0].z * m.rows[3].y - 
                     m.rows[3].x  * m.rows[0].y * m.rows[1].z + 
                     m.rows[3].x  * m.rows[0].z * m.rows[1].y;

    // 3
    inv.rows[0].w = -m.rows[0].y * m.rows[1].z * m.rows[2].w + 
                     m.rows[0].y * m.rows[1].w * m.rows[2].z + 
                     m.rows[1].y * m.rows[0].z * m.rows[2].w - 
                     m.rows[1].y * m.rows[0].w * m.rows[2].z - 
                     m.rows[2].y * m.rows[0].z * m.rows[1].w + 
                     m.rows[2].y * m.rows[0].w * m.rows[1].z;

    // 7
    inv.rows[1].w = m.rows[0].x * m.rows[1].z * m.rows[2].w - 
                    m.rows[0].x * m.rows[1].w * m.rows[2].z - 
                    m.rows[1].x * m.rows[0].z * m.rows[2].w + 
                    m.rows[1].x * m.rows[0].w * m.rows[2].z + 
                    m.rows[2].x * m.rows[0].z * m.rows[1].w - 
                    m.rows[2].x * m.rows[0].w * m.rows[1].z;

    // 11
    inv.rows[2].w = -m.rows[0].x * m.rows[1].y * m.rows[2].w + 
                     m.rows[0].x * m.rows[1].w * m.rows[2].y + 
                     m.rows[1].x * m.rows[0].y * m.rows[2].w - 
                     m.rows[1].x * m.rows[0].w * m.rows[2].y - 
                     m.rows[2].x * m.rows[0].y * m.rows[1].w + 
                     m.rows[2].x * m.rows[0].w * m.rows[1].y;

    // 15
    inv.rows[3].w = m.rows[0].x * m.rows[1].y * m.rows[2].z - 
                    m.rows[0].x * m.rows[1].z * m.rows[2].y - 
                    m.rows[1].x * m.rows[0].y * m.rows[2].z + 
                    m.rows[1].x * m.rows[0].z * m.rows[2].y + 
                    m.rows[2].x * m.rows[0].y * m.rows[1].z - 
                    m.rows[2].x * m.rows[0].z * m.rows[1].y;

    det = m.rows[0].x * inv.rows[0].x + m.rows[0].y * inv.rows[1].x + m.rows[0].z * inv.rows[2].x + m.rows[0].w * inv.rows[3].x;

    if (det == 0)
    {
        return false;
    }

    det = 1.0 / det;

    for (i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if(j%4 == 0)
            {
                invOut->rows[i].x = inv.rows[i].x * det;
            }
            else if(j%4 == 1)
            {
                invOut->rows[i].y = inv.rows[i].y * det;
            }
            else if(j%4 == 2)
            {
                invOut->rows[i].z = inv.rows[i].z * det;
            }
            else if(j%4 == 3)
            {
                invOut->rows[i].w = inv.rows[i].w * det;
            }
        }
    }


    return true;
}

extern inline m4x4 m4x4_translation_matrix(v3 v)
{
    m4x4 result = {};

	result.rows[0] = V4(1.0, 0.0, 0.0, v.x);
	result.rows[1] = V4(0.0, 1.0, 0.0, v.y);
	result.rows[2] = V4(0.0, 0.0, 1.0, v.z);
	result.rows[3] = V4(0.0, 0.0, 0.0, 1.0);

    return(result);
}

extern inline m4x4 m4x4_scale_matrix(v3 v)
{
    m4x4 result = {};

	result.rows[0] = V4(v.x, 0.0, 0.0, 0.0);
	result.rows[1] = V4(0.0, v.y, 0.0, 0.0);
	result.rows[2] = V4(0.0, 0.0, v.z, 0.0);
	result.rows[3] = V4(0.0, 0.0, 0.0, 1.0);

    return(result);
}

extern inline m4x4 m4x4_rotateX_matrix(f32 r)
{
    m4x4 result = {};

	result.rows[0] = V4(1.0, 0.0, 0.0, 0.0);
	result.rows[1] = V4(0.0, cosf(r), -sinf(r), 0.0);
	result.rows[2] = V4(0.0, sinf(r), cosf(r), 0.0);
	result.rows[3] = V4(0.0, 0.0, 0.0, 1.0);

    return(result);
}

extern inline m4x4 m4x4_rotateY_matrix(f32 r)
{
    m4x4 result = {};

	result.rows[0] = V4(cosf(r), 0.0, sinf(r), 0.0);
	result.rows[1] = V4(0.0, 1.0, 0.0, 0.0);
	result.rows[2] = V4(-sinf(r), 0.0, cosf(r), 0.0);
	result.rows[3] = V4(0.0, 0.0, 0.0, 1.0);

    return(result);
}

extern inline m4x4 m4x4_rotateZ_matrix(f32 r)
{
    m4x4 result = {};

	result.rows[0] = V4(cosf(r), -sinf(r), 0.0, 0.0);
	result.rows[1] = V4(sinf(r), cosf(r), 0.0, 0.0);
	result.rows[2] = V4(0.0, 0.0, 1.0, 0.0);
	result.rows[3] = V4(0.0, 0.0, 0.0, 1.0);

    return(result);
}

extern inline v4 m4x4_mul_v4(m4x4 mat, v4 v)
{
    v4 result = V4((mat.rows[0].x * v.x) + (mat.rows[0].y * v.y) + (mat.rows[0].z * v.z) + (mat.rows[0].w * v.w),
                   (mat.rows[1].x * v.x) + (mat.rows[1].y * v.y) + (mat.rows[1].z * v.z) + (mat.rows[1].w * v.w),
                   (mat.rows[2].x * v.x) + (mat.rows[2].y * v.y) + (mat.rows[2].z * v.z) + (mat.rows[2].w * v.w),
                   (mat.rows[3].x * v.x) + (mat.rows[3].y * v.y) + (mat.rows[3].z * v.z) + (mat.rows[3].w * v.w));

    return(result);
}

extern inline m4x4 view_transform(v4 from, v4 to, v4 up)
{
    m4x4 result = {};
    v4 forward = v4_normalize(v4_sub(to, from));
    v4 left = v4_cross(forward, v4_normalize(up));
    v4 true_up = v4_cross(left, forward);

    m4x4 orientation = {};
    orientation.rows[0] = V4(left.x, left.y, left.z, 0);
    orientation.rows[1] = V4(true_up.x, true_up.y, true_up.z, 0);
    orientation.rows[2] = V4(-forward.x,-forward.y,-forward.z,0);
    orientation.rows[3] = V4(0, 0, 0, 1);

    m4x4 translation = {};
    translation.rows[0] = V4(1, 0, 0, -from.x);
    translation.rows[1] = V4(0, 1, 0, -from.y);
    translation.rows[2] = V4(0, 0, 1, -from.z);
    translation.rows[3] = V4(0, 0, 0, 1);

    result = m4x4_mul(orientation, translation);
    
    return(result);
}


extern inline v4 v4_transform(m4x4 m, v4 v)
{
    v4 result = m4x4_mul_v4(m, v);
    return(result);
}

extern inline v4 v4_reflect(v4 I, v4 N) 
{
    v4 result = {};
    result = v4_sub(I, v4_scalar_mul(N, (2.0f * v4_dot(I, N))));
    return(result);
}

#endif
