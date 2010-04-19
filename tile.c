#include "tile.h"
#include <float.h>

#define NTILES 64

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
static vec3 t_dest;

void tiles_init()
{
	int i;

	for(i = 0; i < NTILES; i++) {
		tiles[i].p = vec3_rand();
		tiles[i].v = vec3_zero;
		tiles[i].r = vec3_zero;
	}

	t_dest = vec3_rand();

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
	vec3 col;

	glPushMatrix();
	glTranslated(tile->p.v[0], tile->p.v[1], tile->p.v[2]);
	glScaled(0.1, 0.1, 0.1);

	col = vec3_normalize(vec3_add(tile->v, vec3_scale(tile->p, 0.1)));
	glColor3d(fabs(col.v[0]), fabs(col.v[1]), fabs(col.v[2]));
	glCallList(tile_dlist);

	glPopMatrix();
}

void tiles_draw()
{
	int i;

	glMatrixMode(GL_MODELVIEW);
	for(i = 0; i < NTILES; i++)
		draw_one(&tiles[i]);

	glPushMatrix();
	glTranslated(t_dest.v[0], t_dest.v[1], t_dest.v[2]);
	glScaled(0.1, 0.1, 0.1);
	glCallList(reticle_dlist);
	glPopMatrix();
}

void tiles_change_dest()
{
	t_dest = vec3_rand();
}

static inline vec3 calc_force(int idx)
{
	return vec3_zero;
}

void tiles_update(double dt)
{
	int i;

	for(i = 0; i < NTILES; i++) {
		vec3 dv = vec3_scale(calc_force(i), dt);
		tiles[i].p = vec3_add(vec3_scale(tiles[i].v, dt), tiles[i].p);
		tiles[i].v = vec3_add(tiles[i].v, dv);
	}
}
