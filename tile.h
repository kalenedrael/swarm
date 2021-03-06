/** @file tile.h
 *  @brief tile declarations
 *  @author Chris Lu <czl@andrew>
 */

#ifndef _TILE_H_
#define _TILE_H_

#include "globals.h"
#include "vector.h"

/** @brief tile structure */
typedef struct tile_t {
	vec3 p;  /* position */
	vec3 v;  /* velocity */
	vec3 f;  /* force */
} tile_t;

enum {
	INTEG_EULER,
	INTEG_MIDPT,
	INTEG_RK4
};

/** @brief */
extern vec3 tiles_dest;

void tiles_init();
void tiles_draw(int follow_cam, int integ);
void tiles_update(real_t dt, int integ);
void tiles_change_dest();

#endif /* _TILE_H_ */
