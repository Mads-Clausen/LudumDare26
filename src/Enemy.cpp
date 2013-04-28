#include "Enemy.hpp"

Mix_Chunk *_shot_sound, *_dead_sound, *_hit_sound;

bool has_seeded;

Enemy::Enemy()
{
    _speed = 0;
}

Enemy::~Enemy()
{
    _projectiles.clear();
}

bool Enemy::init()
{
    _has_tex = false;

    if(!has_seeded)
    {
        srand(time(0));
        has_seeded = true;
    }

    int r = (((double) rand()) / RAND_MAX) * 25;
    if(r == 12)
    {
        _has_tex = true;
        _tex = create_gl_surface("res/enemy_potato.bmp", _w, _h);
        if(_w == -1 || _h == -1)
        {
            printf("Unable to initialise Enemy: failed to load texture.");
            return false;
        }
    }

    _w = _h = 24;
    _health = 3;
    _tier = 1;

    _shoot_timer.start();

    if(_shot_sound == 0 || _dead_sound == 0 || _hit_sound == 0)
    {
        _shot_sound = Mix_LoadWAV("res/enemy_shoot_01.wav");
        if(_shot_sound == 0)
        {
            std::cout << "Could not load sound: " << Mix_GetError() << std::endl;
            return false;
        }

        _dead_sound = Mix_LoadWAV("res/enemy_dead_01.wav");
        if(_dead_sound == 0)
        {
            std::cout << "Could not load sound: " << Mix_GetError() << std::endl;
            return false;
        }

        _hit_sound = Mix_LoadWAV("res/enemy_hit_01.wav");
        if(_hit_sound == 0)
        {
            std::cout << "Could not load sound: " << Mix_GetError() << std::endl;
            return false;
        }
    }

    return true;
}

void Enemy::render()
{
    glBindTexture(GL_TEXTURE_2D, 0);

    glPushMatrix();
        glTranslatef(_x + _w / 2, _y + _h / 2, 0.0f);
        glRotatef(_rot, 0.0f, 0.0f, 1.0f);

        if(!_has_tex)
        {
            glBegin(GL_TRIANGLES);
                glColor3f(1.0f, 0.0f, 0.0f);
                glVertex2i(      0,   -_h);
                glVertex2i( _w / 2,    0);
                glVertex2i(-_w / 2,    0);
            glEnd();
        }
        else
        {
            glColor3f(1.0f, 1.0f, 1.0f);
            glBindTexture(GL_TEXTURE_2D, _tex);
            glBegin(GL_QUADS);
                glTexCoord2i(0, 0);
                glVertex2i(-_w / 2, -_h);

                glTexCoord2i(1, 0);
                glVertex2i(_w / 2,  -_h);

                glTexCoord2i(1, 1);
                glVertex2i(_w / 2,  0);

                glTexCoord2i(0, 1);
                glVertex2i(-_w / 2, 0);
            glEnd();
            glBindTexture(GL_TEXTURE_2D, 0);
        }
    glPopMatrix();

    for(unsigned int i = 0; i < _projectiles.size(); ++i)
    {
        _projectiles[i].render();
    }
}

void Enemy::update()
{
    if(!_move_timer.is_started())
    {
        float x = (_player->get_x() + _player->get_w() / 2) - (_x + _w / 2);
        float y = (_player->get_y() + _player->get_h() / 2) - (_y + _h / 2);

        float len = sqrt(x*x + y*y);

        _stepx = x / len;
        _stepy = y / len;

        _move_timer.start();
    }

    for(unsigned int i = 0; i < _projectiles.size(); ++i)
    {
        _projectiles[i].update();

        if(_projectiles[i].x < 0 || _projectiles[i].x > _screen->w ||
           _projectiles[i].y < 0 || _projectiles[i].y > _screen->h)
        {
            _projectiles.erase(_projectiles.begin() + i--);

            for(unsigned int x = i; x < _projectiles.size(); ++x)
                --(_projectiles[x].parent_index);
        }
    }

    _num_projectiles = _projectiles.size();

    if(_player != 0)
    {
        if(_speed != 0)
        {
            if(_stepx == _stepx && _stepy == _stepy)
            {
                _x += _stepx * _speed * timing::get_deltatime();
                _y += _stepy * _speed * timing::get_deltatime();
            }
        }

        if(_move_timer.get_ticks() / 1000.0f > 1.0f)
        {
            float x = (_player->get_x() + _player->get_w() / 2) - (_x + _w / 2);
            float y = (_player->get_y() + _player->get_h() / 2) - (_y + _h / 2);

            float len = sqrt(x*x + y*y);

            _stepx = x / len;
            _stepy = y / len;

            _move_timer.start();
        }

        _speed = 100 * _tier;

        int dx = (_player->get_x() + _player->get_w() / 2) - (_x + _w / 2);
        int dy = (_y + _h / 2) - (_player->get_y() + _player->get_h() / 2);
        _rot = _rot_to = atan2(-dy, dx) * 180 / PI + 90;

        if(_rot < _rot_to - 2 || _rot > _rot_to + 2)
        {
            _rot += (_rot < _rot_to ? 100 : -100) * timing::get_deltatime();
        }
        else
        {
            _rot = _rot_to;
        }

        if(_shoot_timer.get_ticks() / (3000.0f / (_tier * 1.5)) > 1)
        {
            this->shoot();
            _shoot_timer.start();
        }
    }
}

void Enemy::shoot()
{
    //*
    if(Mix_PlayChannel(-1, _shot_sound, 0) == -1)
    {
        std::cout << "Unable to play sound: " << Mix_GetError() << std::endl;
    }

    float x = (_x + _w / 2) - (_player->get_x() + _player->get_w() / 2);
    float y = (_y + _h / 2) - (_player->get_y() + _player->get_h() / 2);

    float len = sqrt(x*x + y*y);
    float stepx = -x / len;
    float stepy = -y / len;

    // std::cout << "Shooting projectile with direction (" << stepx << ", " << stepy << ")" << std::endl;

    Projectile p;
    p.x = (_x + _w / 2);
    p.y = (_y + _h / 2);
    p.stepx = stepx;
    p.stepy = stepy;
    p.speed = 750;

    p.col_r = 1.0f;
    p.col_g = 0.0f;
    p.col_b = 1.0f;

    _projectiles.push_back(p);
    p.parent_index = _projectiles.size() - 1;
    //*/
}

bool Enemy::check_collision(Projectile *p)
{
    if(p->x > _x && p->x < _x + _w &&
       p->y > _y && p->y < _y + _h)
    {
        --_health;

        _w -= ((_tier - 1 < 1 ? 2 : (_tier - 1) * 2) + 1);
        _h -= (_tier + 1);

        if(_w < 16)  _w = 16;
        if(_h < 16)  _h = 16;

        ParticleSystem *_hit_psys;

        if(_health > 0)
        {
            if(Mix_PlayChannel(-1, _hit_sound, 0) == -1)
            {
                std::cout << "Unable to play sound: " << Mix_GetError() << std::endl;
            }

            _hit_psys = new ParticleSystem(_x + _w / 2, _y + _h / 2, 10, 0.0001f, (((double) rand()) / RAND_MAX) * 360, 20, 5, 1.0f, 0.0f, 0.0f, 600, 4, true);
        }
        else
        {
            if(Mix_PlayChannel(-1, _dead_sound, 0) == -1)
            {
                std::cout << "Unable to play sound: " << Mix_GetError() << std::endl;
            }

            _hit_psys = new ParticleSystem(_x + _w / 2, _y + _h / 2, 20, 0.0001f, 0, 360, 100, 1.0f, 0.0f, 0.0f, 400, 4, true);
        }
        _hit_psys->start();
        ParticleManager::register_system(_hit_psys);

        return true;
    }

    return false;
}
