#ifndef PARTICLES_INCLUDED
#define PARTICLES_INCLUDED

#include <SDL/SDL_opengl.h>
#include <ctime>
#include <cstdlib>
#include <vector>
#include <iostream>
#include <cmath>

#include "Timing.hpp"

class Particle
{
    private:
        int _cur_frame;
        bool _dead;

    public:
        float x, y, w, h, stepx, stepy, speed, col_r, col_g, col_b, live_frames, gravity;
        float rot;

        Particle()
        {
            _dead = false;
            w = h = 4;
            _cur_frame = 0;
        };
        virtual ~Particle() {};

        void render();
        void update();

        bool is_dead()
        {
            return _dead;
        }
};

class ParticleSystem
{
    private:
        std::vector<Particle> _particles;
        int _x, _y, _max_particles, _cur_particles, _particle_life, _num_per_cycle;
        float _delay, _rot, _spread, col_r, col_g, col_b, _speed;
        bool _done, _particles_done, _gravity;

        timing::Timer _timer;

    public:
        ParticleSystem();
        ParticleSystem(int x, int y, int m, float d, float ro, float s, int pl, float r, float g, float b, float spd = 400, int npc = 0, bool grav = false);

        void update();
        void render();

        void start()
        {
            _particles.clear();
            _done = _particles_done = false;
            _cur_particles = 0;
            _timer.start();
        }

        bool is_done()
        {
            return _particles_done;
        }
};

class ParticleManager
{
    private:
        static std::vector<ParticleSystem*> _reg;

    public:
        static void register_system(ParticleSystem *s);
        static void update();
        static void render();
};

#endif // PARTICLES_INCLUDED
