/** @file tile.c
 *  @brief tile-related functions
 *  @author Chris Lu <czl@andrew>
 */

#include "tile.h"
#include <float.h>
#include <assert.h>

#define NTILES 512

#define DAMP_FACTOR  0.1   /* weight of damping term */
#define GOAL_FACTOR  1.0   /* weight of goal-seeking term */
#define SEP_FACTOR   0.14  /* weight of separation/cohesion term */
#define CO_FACTOR    1.0   /* weight of separation/cohesion term */
#define ALIGN_FACTOR 0.1   /* weight of alignment term */
#define SEP_DIST     1.2   /* square of radius within which separation applies */
#define CO_DIST      2.0   /* square of radius within which cohesion applies */
#define ALIGN_DIST   0.25  /* square of radius within which alignment applies */
#define SEP_MAX      10.0  /* maximum magnitude of separation force */

#define SCALE_F 0.15       /* factor to scale tiles by */

/** @brief tile vertex array */
static double t_points[11][3] = {
	{  0.0,  0.0,  1.0 }, {  0.0,  0.2,  0.5 }, {  0.0,  0.0,  0.0 },
	{  0.0, -0.2,  0.5 }, {  0.0,  0.0,  1.0 }, { -0.5,  0.0,  0.0 },
	{ -0.5,  0.0, -1.0 }, {  0.0,  0.0,  0.0 }, {  0.5,  0.0, -1.0 },
	{  0.5,  0.0,  0.0 }, {  0.0,  0.0,  1.0 }
};

/** @brief reticle vertex array */
static double ret_points[14][3] = {
	{ 0.5, 0.0, 0.0 }, {-0.5,  0.0,  0.0},
	{ 0.0, 0.5, 0.0 }, { 0.0, -0.5,  0.0},
	{ 0.0, 0.0, 0.5 }, { 0.0,  0.0, -0.5},
	{ 1.0,  1.0,  1.0 }, { -1.0,  1.0,  1.0},
	{ 1.0, -1.0,  1.0 }, { -1.0, -1.0,  1.0},
	{ 1.0, -1.0, -1.0 }, { -1.0, -1.0, -1.0},
	{ 1.0,  1.0, -1.0 }, { -1.0,  1.0, -1.0}
};

/** @brief reticle element list */
static GLubyte ret_elem[30] = {
	0, 1, 2, 3, 4, 5, 6, 8, 8, 10, 10, 12, 12, 6,
	7, 9, 9, 11, 11, 13, 13, 7, 6, 7, 8, 9, 10, 11, 12, 13
};

static tile_t tiles[NTILES];
static int tile_dlist, flake_dlist, reticle_dlist;  /* display lists */
vec3 tiles_dest;                                    /* goal point */

/** @brief initializes the swarm */
void tiles_init()
{
	int i;

	for(i = 0; i < NTILES; i++) {
		tiles[i].p = vec3_rand();
		tiles[i].v = vec3_zero;
	}

	tiles_dest = vec3_scale(vec3_rand(), 3.0);

	glEnableClientState(GL_VERTEX_ARRAY);
	tile_dlist = glGenLists(1);
	glNewList(tile_dlist, GL_COMPILE);
	glVertexPointer(3, GL_DOUBLE, 0, t_points);
	glDrawArrays(GL_LINE_STRIP, 0, sizeof(t_points)/(sizeof(double) * 3));
	glEndList();

	reticle_dlist = glGenLists(1);
	glNewList(reticle_dlist, GL_COMPILE);
	glColor3f(1.0, 1.0, 1.0);
	glVertexPointer(3, GL_DOUBLE, 0, ret_points);
	glDrawElements(GL_LINES, 30, GL_UNSIGNED_BYTE, ret_elem);
	glEndList();

	flake_dlist = glGenLists(1);
	glNewList(flake_dlist, GL_COMPILE);
	glVertexPointer(3, GL_DOUBLE, 0, t_points);
	glDrawArrays(GL_POLYGON, 0, 4);
	glEndList();
	glDisableClientState(GL_VERTEX_ARRAY);

}

/** @brief draws the given tile
 *  @param tile the tile to draw
 */
static void draw_one(tile_t *tile)
{
	vec3 col, nf, nv;
	double yaw, roll, hmag, pitch, fnv;

	/* orient in horizontal plane first */
	yaw = atan2(tile->v.v[0], tile->v.v[2]);
	/* orient in vertical plane */
	hmag = sqrt(tile->v.v[0] * tile->v.v[0] + tile->v.v[2] * tile->v.v[2]);
	pitch = atan2(-tile->v.v[1], hmag);

	/* simulate banking 
	 * first, we figure out what the component of force normal to the
	 * velocity and y is, then we figure out what angle to roll and which
	 * direction to roll in by computing whether the cross of y and force
	 * component is in the same direction as the velocity
	 *
	 * this is probably much more complex than it needs to be
	 */
	nv = vec3_cross(tile->v, vec3_y);
	nf = vec3_component(tile->f, nv);
	fnv = vec3_dot(vec3_cross(vec3_y, nf), tile->v);
	roll = atan2(vec3_mag(nf), 1.0);
	if(fnv < 0.0)
		roll = -roll;

	glPushMatrix();
	glTranslated(tile->p.v[0], tile->p.v[1], tile->p.v[2]);
	glScaled(SCALE_F, SCALE_F, SCALE_F);
	glRotated((180.0 / M_PI) * yaw, 0.0, 1.0, 0.0);
	glRotated((180.0 / M_PI) * pitch, 1.0, 0.0, 0.0);
	glRotated((180.0 / M_PI) * roll, 0.0, 0.0, 1.0);

	col = vec3_scale(tile->v, 1.5);
	glColor3d(fabs(col.v[0]), fabs(col.v[1]), fabs(col.v[2]));
	glCallList(tile_dlist);
	col = tile->f;
	glColor3d(fabs(col.v[0]), fabs(col.v[1]), fabs(col.v[2]));
	glCallList(flake_dlist);

	glPopMatrix();
}

/** @brief draws all tiles */
void tiles_draw()
{
	int i;

	glMatrixMode(GL_MODELVIEW);
	for(i = 0; i < NTILES; i++)
		draw_one(&tiles[i]);

	glPushMatrix();
	glTranslated(tiles_dest.v[0], tiles_dest.v[1], tiles_dest.v[2]);
	glScaled(0.15, 0.15, 0.15);
	glCallList(reticle_dlist);
	glPopMatrix();
}

/** @brief generates a new random destination */
void tiles_change_dest()
{
	tiles_dest = vec3_scale(vec3_rand(), 3.0);
}

/** @brief calculates the goal-seeking force on a tile
 *  @param tile the tile to calculate force for
 *  @return the force to guide the tile toward the goal
 */
static inline vec3 calc_goal_force(tile_t *tile)
{
	vec3 goal_d;

	/* goal force is distance minus speed */
	goal_d  = vec3_sub(tiles_dest, tile->p);
	return vec3_scale(vec3_sub(goal_d, tile->v), GOAL_FACTOR);
}

/** @brief calculates the total force on a tile
 *  @param tile the tile to calculate forces for
 *  @param v_tot the total velocity of all tiles
 *  @return the cumulative force on the tile
 */
static vec3 calc_force(tile_t *tile, vec3 v_tot)
{
	vec3 sep_f = vec3_zero, co_f = vec3_zero, loc_v = tile->v;
	vec3 damp_f, goal_f, loc_f, align_f;
	double m;
	int i, align_n = 0;

	/* calculate local forces */
	for(i = 0; i < NTILES; i++) {
		if(&tiles[i] == tile)
			continue;

		vec3 dir = vec3_sub(tile->p, tiles[i].p);
		double d2 = vec3_mag2(dir);

		/* accumulate cohesion forces */
		if(d2 < CO_DIST)
			co_f = vec3_add(co_f, dir);

		/* accumulate separation forces */
		if(d2 < SEP_DIST)
			sep_f = vec3_add(sep_f, vec3_scale(dir, 1.0 / (d2 * d2)));

		/* accumulate velocities for alignment */
		if(d2 < ALIGN_DIST) {
			loc_v = vec3_add(loc_v, tiles[i].v);
			align_n++;
		}
	}
	co_f = vec3_scale(co_f, CO_FACTOR / (NTILES - 1));

	/* clamp the separation force to prevent explosion */
	if((m = vec3_mag2(sep_f)) > SEP_MAX * SEP_MAX)
		sep_f = vec3_scale(vec3_normalize(sep_f), SEP_MAX);

	/* make alignment force normal to the current velocity */
	if(align_n != 0) {
		loc_v = vec3_scale(loc_v, 1.0 / (double)align_n);
		align_f = vec3_make_norm(vec3_scale(loc_v, ALIGN_FACTOR), tile->v);
	}
	else {
		align_f = vec3_zero;
	}

	/* separation factor increases with velocity for effect */
	loc_f = vec3_add(vec3_neg(co_f), vec3_scale(sep_f, SEP_FACTOR + vec3_mag(v_tot) * 0.2));
	damp_f = vec3_scale(vec3_neg(tile->v), DAMP_FACTOR);
	goal_f = calc_goal_force(tile);

	return vec3_add(align_f, vec3_add(loc_f, vec3_add(goal_f, damp_f)));
}

/** @brief updates tile positions and velocities
 *  @param dt the timestep to update for
 */
void tiles_update(double dt)
{
	int i;
	vec3 v_tot = vec3_zero;

	/* accumulate velocities */
	for(i = 0; i < NTILES; i++)
		v_tot = vec3_add(v_tot, tiles[i].v);

	v_tot = vec3_scale(v_tot, 1.0 / NTILES);

	/* Euler integrate for each tile */
	for(i = 0; i < NTILES; i++) {
		tiles[i].f = calc_force(&tiles[i], v_tot);
		tiles[i].p = vec3_add(vec3_scale(tiles[i].v, dt), tiles[i].p);
		tiles[i].v = vec3_add(vec3_scale(tiles[i].f, dt), tiles[i].v);
	}
}
