#ifndef RENDER_INCLUDED
#define RENDER_INCLUDED

#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <stdio.h>

GLuint create_gl_surface(const char *tex_path, int &w, int &h);

class Drawable
{
    public:
        static SDL_Surface *_screen;

        Drawable() {};
        virtual ~Drawable() {};

        virtual void render() {};
};

#endif // RENDER_INCLUDED
