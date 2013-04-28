#include "Particles.hpp"

bool is_seeded;

void Particle::render()
{
    if(!_dead)
    {
        glBindTexture(GL_TEXTURE_2D, 0);

        glPushMatrix();
            glTranslatef(x + w / 2, y + h / 2, 0.0);
            glColor3f(col_r, col_g, col_b);
            glRotatef(rot, 0.0f, 0.0f, 1.0f);

            glBegin(GL_TRIANGLES);
                glVertex2f(0, -h / 2);
                glVertex2f(w / 2, h / 2);
                glVertex2f(-w / 2, h / 2);
            glEnd();
        glPopMatrix();
    }
}

void Particle::update()
{
    rot += 3;

    if(rot > 360)
        rot = 0;

    x += stepx * speed * timing::get_deltatime();
    y += stepy * speed * timing::get_deltatime();

    if(gravity)
        stepy -= abs(stepy) * 9.8;

    ++_cur_frame;

    if(_cur_frame >= live_frames)
    {
        _dead = true;
    }
}

ParticleSystem::ParticleSystem()
{
    _max_particles = 10;
    _delay = 0.2f;
    col_r = col_g = col_b = 1.0f;
    _cur_particles = 0;
    _rot = 0;
    _particle_life = 100;
    _spread = 0;
    _speed = 400;
    _num_per_cycle = 1;
    _gravity = false;

    if(!is_seeded)
    {
        srand(time(0));
        is_seeded = true;
    }

    _done = _particles_done = false;
}

ParticleSystem::ParticleSystem(int x, int y, int m, float d, float ro, float s, int pl, float r, float g, float b, float spd, int npc, bool grav)
{
    _x = x;
    _y = y;
    _max_particles = m;
    _delay = d;
    _rot = ro;
    _spread = s;
    _particle_life = pl;
    col_r = r;
    col_g = g;
    col_b = b;
    _cur_particles = 0;
    _speed = spd;
    _num_per_cycle = npc;
    _gravity = grav;

    if(!is_seeded)
    {
        srand(time(0));
        is_seeded = true;
    }

    _done = _particles_done = false;
}

void ParticleSystem::render()
{
    if(!_particles_done)
    {
        for(unsigned int i = 0; i< _particles.size(); ++i)
            _particles[i].render();
    }
}

void ParticleSystem::update()
{
    if(!_particles_done)
    {
        if(!_done && _timer.get_ticks() / 1000.0f > _delay)
        {
            for(int _n = 0; _n < _num_per_cycle; ++_n)
            {
                int r = (((double) rand()) / RAND_MAX) * _spread;
                r -= _spread / 2;
                r += _rot;

                Particle p;
                p.x = _x;
                p.y = _y;
                p.stepx = cos(r);
                p.stepy = sin(r);
                p.speed = _speed;

                p.col_r = col_r;
                p.col_g = col_g;
                p.col_b = col_b;

                p.gravity = _gravity;
                p.live_frames = _particle_life;

                _particles.push_back(p);
                ++_cur_particles;
            }

            _timer.start();
        }

        bool alive = false;

        for(unsigned int i = 0; i < _particles.size(); ++i)
        {
            _particles[i].update();

            if(_particles[i].is_dead())
            {
                _particles.erase(_particles.begin() + i);
            }
            else
            {
                alive = true;
            }
        }

        if(!alive && _done)
            _particles_done = true;

        if(_cur_particles > _max_particles)
        {
            _done = true;
        }
    }
}


std::vector<ParticleSystem*> ParticleManager::_reg;

void ParticleManager::register_system(ParticleSystem *p)
{
    _reg.push_back(p);
}

void ParticleManager::render()
{
    for(unsigned int i = 0; i < _reg.size(); ++i)
    {
        _reg[i]->render();
    }
}

void ParticleManager::update()
{
    for(unsigned int i = 0; i < _reg.size(); ++i)
    {
        _reg[i]->update();

        if(_reg[i]->is_done())
            _reg.erase(_reg.begin() + i--);
    }
}
