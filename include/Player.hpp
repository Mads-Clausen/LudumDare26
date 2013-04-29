#ifndef PLAYER_INCLUDED
#define PLAYER_INCLUDED

#include <cstdio>
#include <vector>
#include <iostream>

#include <SDL/SDL_mixer.h>

#include <cmath>
#define PI 3.14159265

#include "Render.hpp"
#include "Timing.hpp"
#include "Projectile.hpp"
#include "Powerup.hpp"
#include "Particles.hpp"

class Player : public Drawable
{
    private:
        std::vector<Projectile> _projectiles;
        int _x, _y, _w, _h, _init_w, _init_h, _health;
        unsigned int _num_projectiles;
        float _rot, _stepx, _stepy, _speed, _cur_mouse_x, _cur_mouse_y, _shoot_wait, _tier, _old_tier;
        GLuint _tex;

        Mix_Chunk *_shot_sound;
        timing::Timer _shoot_timer;

        bool _mouse_control, _shooting;

        // Powerup stuff
        int _num_extra_shots, _extra_speed, _spd_pu_time, _proj_spd_pu_time;
        float _extra_proj_speed;
        timing::Timer _spd_pu_timer, _proj_spd_pu_timer;

    public:
        Player();
        virtual ~Player();

        bool init(const char *tex_path);

        void update();
        void render();
        void update_rotation(float mouse_x, float mouse_y);
        void shoot(float mouse_x, float mouse_y);
        void on_key_event(SDL_Event *ev);
        void on_mouse_event(SDL_Event *ev);

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

        std::vector<Projectile>& get_projectiles()
        {
            return _projectiles;
        }

        void set_tier(int t)
        {
            _tier = t;
            _health = 10 * t;
        }

        int get_tier() const
        {
            return _tier;
        }

        void add_powerup(POWERUP_TYPE t, int d)
        {
            switch(t)
            {
                case POWERUP_SPEED:
                    _spd_pu_timer.start();
                    _spd_pu_time = (float) d;
                    _extra_speed = 300;
                    break;
                case POWERUP_PROJECTILE_SPEED:
                    _proj_spd_pu_timer.start();
                    _proj_spd_pu_time = (float) d;
                    _extra_proj_speed = 0.1f;
                    break;
                case POWERUP_TRIPLESHOT:
                    _num_extra_shots += d;
                    break;
                case POWERUP_NONE:
                    break;
                default:
                    break;
            }
        }

        bool check_collision(Projectile *p);

        bool is_dead()
        {
            return _health <= 0;
        }

        void set_health(int h)
        {
            _health = h;
        }

        int get_health()
        {
            return _health;
        }
};

#endif // PLAYER_INCLUDED
