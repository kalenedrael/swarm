/** @file swarm.c
 *  @brief swarm main code
 *  @author Chris Lu <czl@andrew>
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "globals.h"
#include "tile.h"
#include "vector.h"

/* SDL stuff */
static SDL_Surface *screen;
static void update();
static void draw();

static int oldtime;                            /* timekeeper */
static int follow_cam;                         /* whether or not to use follow cam */

static vec3 d_pos = {.v = {0.0, 0.0, 0.0}};    /* goal movement */
static vec3 cam_pos = {.v = {2.0, 2.0, 8.0}};  /* camera position */

/** @brief handle a SDL event
 *  @param ev the event to handle
 */
static void handle_event(SDL_Event *ev)
{
	switch(ev->type) {
	case SDL_QUIT:
		exit(0);
	case SDL_MOUSEBUTTONUP:
		return;
	case SDL_KEYDOWN:
		switch(ev->key.keysym.sym) {
		case SDLK_w: d_pos.v[2] = -1.0; break;
		case SDLK_s: d_pos.v[2] =  1.0; break;
		case SDLK_a: d_pos.v[0] = -1.0; break;
		case SDLK_d: d_pos.v[0] =  1.0; break;
		case SDLK_l: d_pos.v[1] = -1.0; break;
		case SDLK_k: d_pos.v[1] =  1.0; break;
		case SDLK_r: tiles_init(); break;
		case SDLK_f: tiles_change_dest(); break;
		case SDLK_g: follow_cam = !follow_cam;
		default: break;
		}
		break;
	case SDL_KEYUP:
		switch(ev->key.keysym.sym) {
		case SDLK_w: case SDLK_s: d_pos.v[2] = 0.0; break;
		case SDLK_a: case SDLK_d: d_pos.v[0] = 0.0; break;
		case SDLK_k: case SDLK_l: d_pos.v[1] = 0.0; break;
		default: break;
		}
	case SDL_USEREVENT: /* update event */
		draw();
		update();
	default:
		return;
	}
}

/** @brief timer callback
 *  @param x the timestep
 */
static Uint32 timer_cb(Uint32 x, void* p)
{
	SDL_Event tev;
	tev.user.type = SDL_USEREVENT;
	SDL_PushEvent(&tev);
	return x;
}

/** @brief updates the scene */
static void update()
{
	int newtime = SDL_GetTicks();
	int idt = newtime - oldtime;
	double dt = (double)idt / 1000.0;

	tiles_dest = vec3_add(tiles_dest, vec3_scale(d_pos, dt * 3.0));
	tiles_update(dt);

	oldtime = newtime;
}

/** @brief draws the scene */
static void draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	if(!follow_cam) {
		gluLookAt(cam_pos.v[0], cam_pos.v[1], cam_pos.v[2],
		          0.0, 0.0, 0.0,
		          0.0, 1.0, 0.0);
	}

	tiles_draw(follow_cam);

	SDL_GL_SwapBuffers();
}

/** @brief initializes SDL, GL, and scene state */
static void init()
{
	/* set up SDL */
	screen = NULL;
	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) {
		printf("Init failed: %s\n", SDL_GetError());
		exit(1);
	}
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
	screen = SDL_SetVideoMode(XRES, YRES, 32, SDL_OPENGL | SDL_HWSURFACE);
	if(!screen) {
		printf("Video mode init failed: %s\n", SDL_GetError());
		exit(1);
	}

	SDL_WM_SetCaption("swarm", "swarm");
	atexit(SDL_Quit);

	srand48(time(NULL));
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_COLOR_MATERIAL);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POLYGON_SMOOTH);
	glLineWidth(1.1);

	glMatrixMode(GL_PROJECTION);
	gluPerspective(60.0, (double)XRES/(double)YRES, 0.001, 200.0);

	/* timer for update and draw */
	oldtime = SDL_GetTicks();
	if(SDL_AddTimer(1000 / FPS, timer_cb, NULL) == NULL) {
		printf("Error setting update timer...\n");
		exit(1);
	}

	tiles_init();
}

int main(int argc, char **argv)
{
	SDL_Event ev;

	init();

	while (SDL_WaitEvent(&ev))
		handle_event(&ev);

	return 1;
}
