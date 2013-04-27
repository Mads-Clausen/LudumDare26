#ifndef PLAYER_INCLUDED
#define PLAYER_INCLUDED

#include <stdio.h>
#include <vector>
#include <iostream>

#include <SDL/SDL_mixer.h>

#include <math.h>
#define PI 3.14159265

#include "Render.hpp"
#include "Projectile.hpp"

class Player : public Drawable
{
    private:
        std::vector<Projectile*> _projectiles;
        int _x, _y, _w, _h;
        unsigned int _num_projectiles;
        float _rot, _stepx, _stepy, _speed, _cur_mouse_x, _cur_mouse_y;
        GLuint _tex;

        Mix_Chunk *_shot_sound;

        bool _mouse_control;

    public:
        Player();
        virtual ~Player();

        bool init(const char *tex_path);

        void update();
        void render();
        void update_rotation(float mouse_x, float mouse_y);
        void shoot(float mouse_x, float mouse_y);
        void on_key_event(SDL_Event *ev);

        void set_position(int x, int y)
        {
            _x = x;
            _y = y;
        }

        int get_x() const
        {
            return _x;
        }

        int get_y() const
        {
            return _y;
        }

        int get_w() const
        {
            return _w;
        }

        int get_h() const
        {
            return _h;
        }

        std::vector<Projectile*>& get_projectiles()
        {
            return _projectiles;
        }
};

#endif // PLAYER_INCLUDED
