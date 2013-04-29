#ifndef RENDER_INCLUDED
#define RENDER_INCLUDED

#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <stdio.h>
#include <math.h>
#define PI 3.14159265
#define DEG2RAD PI/180

GLuint create_gl_surface(const char *tex_path, int &w, int &h);

void draw_circle(float cx, float cy, float radius, int num_segments);

class Drawable
{
    public:
        static SDL_Surface *_screen;

        Drawable() {};
        virtual ~Drawable() {};

        virtual void render() {};
};

#endif // RENDER_INCLUDED
