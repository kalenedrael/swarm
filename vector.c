#include "vector.h"
#include <stdio.h>

const vec3 vec3_zero = { .v = { 0.0, 0.0, 0.0 } };
const vec3 vec3_z = { .v = { 0.0, 0.0, 1.0 } };

/* generates uniformly distributed random vector on unit sphere */
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

void vec3_print(vec3 v)
{
	printf("<%f, %f, %f>\n", v.v[0], v.v[1], v.v[2]);
}
