#ifndef _TILE_H_
#define _TILE_H_

#include "globals.h"
#include "vector.h"

typedef struct tile_t {
	vec3 p, v, f;
} tile_t;

extern vec3 tiles_dest;

void tiles_init();
void tiles_draw();
void tiles_update(double dt);
void tiles_change_dest();

#endif /* _TILE_H_ */
