#ifndef Enemy_INCLUDED
#define Enemy_INCLUDED

#include <cstdio>
#include <vector>
#include <iostream>

#include <SDL/SDL_mixer.h>

#include <cstdlib>
#include <ctime>

#include <cmath>
#define PI 3.14159265

#include "Render.hpp"
#include "Projectile.hpp"
#include "Player.hpp"
#include "Timer.hpp"
#include "Particles.hpp"
#include "Powerup.hpp"

class Enemy : public Drawable
{
    private:
        std::vector<Projectile> _projectiles;
        int _x, _y, _w, _h, _health;
        unsigned int _num_projectiles;
        float _rot, _rot_to, _stepx, _stepy, _speed, _tier;
        GLuint _tex;
        bool _has_tex;

        Player *_player;

        timing::Timer _shoot_timer, _move_timer;

    public:
        Enemy();
        virtual ~Enemy();

        bool init();

        void update();
        void render();
        void shoot();
        bool check_collision(Projectile *p);

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

        void set_size(int w, int h)
        {
            _w = w;
            _h = h;
        }

        int get_w() const
        {
            return _w;
        }

        int get_h() const
        {
            return _h;
        }

        void set_target_player(Player *pl)
        {
            _player = pl;
        }

        bool is_dead()
        {
            return _health <= 0;
        }

        void set_tier(float t)
        {
            _health = (int) (t - 0.5f) + 2;
            _tier = t;
        }

        float get_tier() const
        {
            return _tier;
        }

        std::vector<Projectile>& get_projectiles()
        {
            return _projectiles;
        }
};

#endif // Enemy_INCLUDED
