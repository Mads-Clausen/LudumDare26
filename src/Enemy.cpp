#include "Enemy.hpp"

Mix_Chunk *_shot_sound;

Enemy::Enemy()
{
    _speed = 0;
}

Enemy::~Enemy()
{
    for(unsigned int i = 0; i < _projectiles.size(); ++i)
    {
        delete _projectiles[i];
    }
}

bool Enemy::init(const char *tex_path)
{
    /*
    _tex = create_gl_surface(tex_path, _w, _h);
    if(_w == -1 || _h == -1)
    {
        printf("Unable to initialise Enemy: failed to load texture.");
        return false;
    }

    printf("%i * %i\n", _w, _h);
    */
    _w = _h = 24;
    _health = 3;
    _tier = 1;

    _shoot_timer.start();

    if(_shot_sound == 0)
    {
        _shot_sound = Mix_LoadWAV("res/enemy_shoot_01.wav");
        if(_shot_sound == 0)
        {
            std::cout << "Could not load sound: " << Mix_GetError() << std::endl;
            return false;
        }
    }

    return true;
}

void Enemy::render()
{
    glPushMatrix();
        glTranslatef(_x + _w / 2, _y + _h / 2, 0.0f);
        glRotatef(_rot, 0.0f, 0.0f, 1.0f);

        glBegin(GL_TRIANGLES);
            glColor3f(1.0f, 0.0f, 0.0f);
            glVertex2i(      0,   -_h);
            glVertex2i( _w / 2,    0);
            glVertex2i(-_w / 2,    0);
        glEnd();
    glPopMatrix();

    for(unsigned int i = 0; i < _projectiles.size(); ++i)
    {
        _projectiles[i]->render();
    }
}

void Enemy::update()
{
    for(unsigned int i = 0; i < _projectiles.size(); ++i)
    {
        if(_projectiles.size() != _num_projectiles)
        {
            _projectiles[i]->parent_index = i;
        }
        _projectiles[i]->update();

        if(_projectiles[i]->x < 0 || _projectiles[i]->x > _screen->w ||
           _projectiles[i]->y < 0 || _projectiles[i]->y > _screen->h)
        {
            delete _projectiles[i];
            _projectiles.erase(_projectiles.begin() + i);

            for(unsigned int x = i; x < _projectiles.size(); ++x)
                --(_projectiles[x]->parent_index);
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

        float x = (_player->get_x() + _player->get_w() / 2) - (_x + _w / 2);
        float y = (_player->get_y() + _player->get_h() / 2) - (_y + _h / 2);

        float len = sqrt(x*x + y*y);

        _stepx = x / len;
        _stepy = y / len;

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

        if(_shoot_timer.get_ticks() / (3000.0f / _tier) > 1)
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

    Projectile *p = new Projectile();
    p->x = (_x + _w / 2);
    p->y = (_y + _h / 2);
    p->stepx = stepx;
    p->stepy = stepy;
    p->speed = 750;

    p->col_r = 1.0f - 1.0f / 0.0f;
    p->col_g = 1.0f - 1.0f / 255;
    p->col_b = 1.0f - 1.0f / 0.0f;

    _projectiles.push_back(p);
    p->parent_index = _projectiles.size() - 1;
    //*/
}

bool Enemy::check_collision(Projectile *p)
{
    if(p->x > _x && p->x < _x + _w &&
       p->y > _y && p->y < _y + _h)
    {
        --_health;
        std::cout << _health << std::endl;
        return true;
    }

    return false;
}
