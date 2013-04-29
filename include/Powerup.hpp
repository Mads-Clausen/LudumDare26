#ifndef POWERUP_INCLUDED
#define POWERUP_INCLUDED

#include <SDL/SDL_opengl.h>
#include <iostream>
#include <vector>

#include "Timing.hpp"
#include "Render.hpp"

enum POWERUP_TYPE
{
    POWERUP_NONE,
    POWERUP_SPEED,
    POWERUP_PROJECTILE_SPEED,
    POWERUP_TRIPLESHOT
};

class Powerup
{
    private:
        timing::Timer _timer;
        float col_r, col_g, col_b, _a;
        bool _dead;
        GLuint _tex;

    public:
        int x, y, w, h, d, _time;
        POWERUP_TYPE type;

        Powerup()
        {
            type = POWERUP_NONE;

            int _w = 0, _h = 0;
            _tex = create_gl_surface("res/powerup_none.bmp", _w, _h);

            if(_w < 0 || _h < 0)
            {
                std::cout << "Unable to load texture. Expect errors." << std::endl;
            }

            _time = 7.0f;
            _dead = false;
            _a = 1.0f;
        }

        Powerup(POWERUP_TYPE t, int _x, int _y, int _w, int _h, int _d, int ti)
        {
            type = t;
            x = _x;
            y = _y;
            d = _d;
            _time = ti;
            _dead = false;
            _a = 1.0f;

            switch(type)
            {
                case POWERUP_NONE:
                    _tex = create_gl_surface("res/powerup_none.bmp", w, h);
                    break;
                case POWERUP_PROJECTILE_SPEED:
                    _tex = create_gl_surface("res/powerup_proj_spd.bmp", w, h);
                    break;
                case POWERUP_SPEED:
                    _tex = create_gl_surface("res/powerup_spd.bmp", w, h);
                    break;
                case POWERUP_TRIPLESHOT:
                    _tex = create_gl_surface("res/powerup_tripleshot.bmp", w, h);
                    col_r = 0.0f;
                    col_g = 1.0f - 1.0f / 170.0f;
                    col_b = 1.0f - 1.0f / 201.0f;
                    break;
                default:
                    break;
            }

            if(w < 0 || h < 0)
            {
                std::cout << "Unable to load texture. Expect errors." << std::endl;
            }

            w = _w;
            h = _h;

        }

        void start()
        {
            _timer.start();
        }

        void render();

        void update()
        {
            if(_timer.get_ticks() / 1000.0f > _time)
            {
                _dead = true;
            }
        }

        bool is_dead()
        {
            return _dead;
        }

        void kill()
        {
            _dead = true;
        }
};

class PowerupManager
{
    private:
        static std::vector<Powerup*> _reg;

    public:
        static void register_powerup(Powerup *p);
        static void update();
        static void render();
        static std::vector<Powerup*>& get_powerups();
};

#endif // POWERUP_INCLUDED
