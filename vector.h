/** @file vector.h
 *  @brief vector declarations and functions
 *  @author Chris Lu <czl@andrew>
 */

#ifndef _VECTOR_H_
#define _VECTOR_H_

#include <math.h>
#include <stdlib.h>
#include "globals.h"

typedef struct vec3 {
	real_t v[3];
} vec3;

const vec3 vec3_zero;
const vec3 vec3_z, vec3_y;

static inline vec3 vec3_add(vec3 a, vec3 b)
{
	vec3 ret;
	ret.v[0] = a.v[0] + b.v[0];
	ret.v[1] = a.v[1] + b.v[1];
	ret.v[2] = a.v[2] + b.v[2];
	return ret;
}

static inline vec3 vec3_sub(vec3 a, vec3 b)
{
	vec3 ret;
	ret.v[0] = a.v[0] - b.v[0];
	ret.v[1] = a.v[1] - b.v[1];
	ret.v[2] = a.v[2] - b.v[2];
	return ret;
}

static inline vec3 vec3_neg(vec3 v)
{
	vec3 ret;
	ret.v[0] = -v.v[0];
	ret.v[1] = -v.v[1];
	ret.v[2] = -v.v[2];
	return ret;
}

static inline real_t vec3_dot(vec3 a, vec3 b)
{
	return a.v[0] * b.v[0] + a.v[1] * b.v[1] + a.v[2] * b.v[2];
}

static inline vec3 vec3_cross(vec3 a, vec3 b)
{
	vec3 ret;
	ret.v[0] = a.v[1] * b.v[2] - a.v[2] * b.v[1];
	ret.v[1] = a.v[2] * b.v[0] - a.v[0] * b.v[2];
	ret.v[2] = a.v[0] * b.v[1] - a.v[1] * b.v[0];
	return ret;
}

static inline vec3 vec3_scale(vec3 a, real_t scale)
{
	vec3 ret;
	ret.v[0] = a.v[0] * scale;
	ret.v[1] = a.v[1] * scale;
	ret.v[2] = a.v[2] * scale;
	return ret;
}

/** @brief calculates the squared magnitude of a vector */
static inline real_t vec3_mag2(vec3 v)
{
	return v.v[0] * v.v[0] + v.v[1] * v.v[1] + v.v[2] * v.v[2];
}

static inline real_t vec3_mag(vec3 v)
{
	return sqrt(vec3_mag2(v));
}

static inline vec3 vec3_normalize(vec3 a)
{
	return vec3_scale(a, 1.0 / sqrt(vec3_mag2(a)));
}

vec3 vec3_component(vec3 a, vec3 b);
vec3 vec3_make_norm(vec3 a, vec3 b);
vec3 vec3_rand();
void vec3_print(vec3);

#endif
