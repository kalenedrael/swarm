#include "tile.h"
#include <float.h>

#define NTILES 64
#define NDEST 3

#define SQRT_3_2 0.86602540378443864676
static double t_points[7][3] = {
	{ -1.0, 0.0, 0.0 }, { -0.5, SQRT_3_2, 0.0 }, { 0.5, SQRT_3_2, 0.0 },
	{ 1.0, 0.0, 0.0 }, { 0.5, -SQRT_3_2, 0.0 }, { -0.5, -SQRT_3_2, 0.0 },
	{ -1.0, 0.0, 0.0 }
};

static double ret_points[14][3] = {
	{ 0.5, 0.0, 0.0 }, {-0.5,  0.0,  0.0},
	{ 0.0, 0.5, 0.0 }, { 0.0, -0.5,  0.0},
	{ 0.0, 0.0, 0.5 }, { 0.0,  0.0, -0.5},
	{ 1.0,  1.0,  1.0 }, { -1.0,  1.0,  1.0},
	{ 1.0, -1.0,  1.0 }, { -1.0, -1.0,  1.0},
	{ 1.0, -1.0, -1.0 }, { -1.0, -1.0, -1.0},
	{ 1.0,  1.0, -1.0 }, { -1.0,  1.0, -1.0}
};

static GLubyte ret_elem[30] = {
	0, 1, 2, 3, 4, 5, 6, 8, 8, 10, 10, 12, 12, 6,
	7, 9, 9, 11, 11, 13, 13, 7, 6, 7, 8, 9, 10, 11, 12, 13
};

static tile_t tiles[NTILES];
static int tile_dlist;
static int reticle_dlist;
static vec3 t_dest[NDEST];

void tiles_init()
{
	int i;

	for(i = 0; i < NTILES; i++) {
		tiles[i].p = vec3_rand();
		tiles[i].v = vec3_zero;
		tiles[i].r = vec3_zero;
	}

	for(i = 0; i < NDEST; i++)
		t_dest[i] = vec3_rand();

	glEnableClientState(GL_VERTEX_ARRAY);
	tile_dlist = glGenLists(1);
	glNewList(tile_dlist, GL_COMPILE);
	glVertexPointer(3, GL_DOUBLE, 0, t_points);
	glDrawArrays(GL_LINE_STRIP, 0, 7);
	glEndList();

	reticle_dlist = glGenLists(1);
	glNewList(reticle_dlist, GL_COMPILE);
	glColor3f(1.0, 1.0, 1.0);
	glVertexPointer(3, GL_DOUBLE, 0, ret_points);
	glDrawElements(GL_LINES, 30, GL_UNSIGNED_BYTE, ret_elem);
	glEndList();
	glDisableClientState(GL_VERTEX_ARRAY);

}

static void draw_one(tile_t *tile)
{
	double theta;
	vec3 axis;
	vec3 col;

	axis = vec3_cross(vec3_z, tile->p);
	theta = asin(sqrt(vec3_mag2(axis)));
	if(vec3_dot(vec3_z, tile->p) < 0.0)
		theta = M_PI - theta;

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glRotated((180.0 / M_PI) * theta, axis.v[0], axis.v[1], axis.v[2]);
	glTranslated(0.0, 0.0, 1.0);
	glScaled(0.1, 0.1, 0.1);

	col = vec3_normalize(vec3_add(tile->v, vec3_scale(tile->p, 0.1)));
	glColor3d(fabs(col.v[0]), fabs(col.v[1]), fabs(col.v[2]));
	glCallList(tile_dlist);

	glPopMatrix();
}

void tiles_draw()
{
	int i;

	for(i = 0; i < NTILES; i++)
		draw_one(&tiles[i]);

	for(i = 0; i < NDEST; i++) {
		glPushMatrix();
		glTranslated(t_dest[i].v[0], t_dest[i].v[1], t_dest[i].v[2]);
		glScaled(0.1, 0.1, 0.1);
		glCallList(reticle_dlist);
		glPopMatrix();
	}
}

void tiles_change_dest()
{
	int i;

	for(i = 0; i < NDEST; i++)
		t_dest[i] = vec3_rand();
}

static int find_nearest(vec3 p)
{
	int i, a = 0;
	double near = DBL_MAX;

	for(i = 0; i < NDEST; i++) {
		double m = vec3_mag2(vec3_sub(p, t_dest[i]));
		if(m < near) {
			a = i;
			near = m;
		}
	}
	return a;
}

static inline vec3 make_norm(vec3 v, vec3 n)
{
	return vec3_sub(v, vec3_scale(n, vec3_dot(v, n)));
}

/* dest, pos on unit sphere */
static inline vec3 accel_toward(vec3 dest, vec3 pos)
{
	vec3 d = vec3_sub(dest, pos);
	return vec3_normalize(make_norm(d, pos));
}

static inline vec3 calc_force(int idx)
{
	int j;

	vec3 pos = tiles[idx].p;
	vec3 dest = t_dest[find_nearest(pos)];
	double cpot = acos(vec3_dot(dest, pos));
	vec3 f = vec3_scale(accel_toward(dest, pos), cpot);
	for(j = 0; j < NTILES; j++) {
		if(j == idx)
			continue;
		vec3 tow = accel_toward(tiles[j].p, pos);
		double pot = fmax(acos(vec3_dot(tiles[j].p, pos)), 0.05);
		f = vec3_sub(f, vec3_scale(tow, 0.01 / (pot * pot)));
	}
	return f;
}

void tiles_update(double dt)
{
	int i;

	for(i = 0; i < NTILES; i++) {
		vec3 dv = vec3_scale(calc_force(i), dt);
		tiles[i].p = vec3_normalize(vec3_add(vec3_scale(tiles[i].v, dt), tiles[i].p));
		tiles[i].v = make_norm(vec3_add(vec3_scale(tiles[i].v, 0.975), dv), tiles[i].p);
	}
}
