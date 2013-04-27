#ifndef PROJECTILE_INCLUDED
#define PROJECTILE_INCLUDED

#include <vector>
#include <iostream>

#include <SDL/SDL_opengl.h>

#include "Render.hpp"
#include "Timing.hpp"

class Projectile
{
    public:
        float x, y, w, h, stepx, stepy, speed, col_r, col_g, col_b;
        int parent_index;

        Projectile() : w(2.0f), h(2.0f), col_r(1.0f), col_g(1.0f), col_b(1.0f) {};
        virtual ~Projectile() {};

        void render();

        void update()
        {
            x += stepx * speed * timing::get_deltatime();
            y += stepy * speed * timing::get_deltatime();
        }

        void destroy(std::vector<void*> &parent_container)
        {
            parent_container.erase(parent_container.begin() + parent_index);

            delete this;
        }
};

#endif // PROJECTILE_INCLUDED
