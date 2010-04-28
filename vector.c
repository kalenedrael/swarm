/** @file vector.c
 *  @brief vector manipulation functions
 *  @author Chris Lu <czl@andrew>
 */

#include "vector.h"
#include <stdio.h>

const vec3 vec3_zero = { .v = { 0.0, 0.0, 0.0 } };
const vec3 vec3_z = { .v = { 0.0, 0.0, 1.0 } };

/** @brief generates uniformly distributed random vector on unit sphere */
vec3 vec3_rand()
{
	vec3 ret;
	double theta = drand48() * (2.0 * M_PI);
	double u = drand48() * 2.0 - 1.0;
	double v = sqrt(1.0 - u * u);

	ret.v[0] = v * cos(theta);
	ret.v[1] = v * sin(theta);
	ret.v[2] = u;
	return ret;
}

/** @brief calculates the b component of a
 *  @param a the vector to get the component of
 *  @param b the direction component
 *  @return the normal component
 */
vec3 vec3_component(vec3 a, vec3 b)
{
	double mag2 = vec3_mag2(b);

	if(mag2 == 0.0)
		return vec3_zero;

	vec3 nb = vec3_scale(b, 1.0 / sqrt(mag2));
	return vec3_scale(nb, vec3_dot(a, nb));
}

/** @brief calculates the component of a normal to b
 *  @param a the vector to get the component of
 *  @param b the vector to be normal to
 *  @return the normal component
 */
vec3 vec3_make_norm(vec3 a, vec3 b)
{
	double mag2 = vec3_mag2(b);

	if(mag2 == 0.0)
		return vec3_zero;

	vec3 nb = vec3_scale(b, 1.0 / sqrt(mag2));
	return vec3_sub(a, vec3_scale(nb, vec3_dot(a, nb)));
}

/** @brief prints a vector
 *  @param v the vector to print
 */
void vec3_print(vec3 v)
{
	printf("<%f, %f, %f>\n", v.v[0], v.v[1], v.v[2]);
}
