#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include <SDL/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>

#define XRES 1024
#define YRES 768
#define FPS 60

//#define USE_DOUBLE

#ifdef USE_DOUBLE
typedef double real_t;
#define glTranslate(...) glTranslated(__VA_ARGS__)
#define glScale(...) glScaled(__VA_ARGS__)
#define glRotate(...) glRotated(__VA_ARGS__)
#define glColor4(...) glColor4d(__VA_ARGS__)
#define GL_SIZE GL_DOUBLE
#else
typedef float real_t;
#define glTranslate(...) glTranslatef(__VA_ARGS__)
#define glScale(...) glScalef(__VA_ARGS__)
#define glRotate(...) glRotatef(__VA_ARGS__)
#define glColor4(...) glColor4f(__VA_ARGS__)
#define GL_SIZE GL_FLOAT
#endif /* USE_DOUBLE */

#endif /* GLOBALS_H */
