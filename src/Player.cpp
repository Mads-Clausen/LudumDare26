#include "Player.hpp"

// float temp_x, temp_y;

Player::Player()
{
    _speed = 0;
    _tier = 1;
    _extra_proj_speed = _extra_speed = _num_extra_shots = 0;
    _mouse_control = true;
    _shoot_wait = 0.27f;
}

Player::~Player()
{
    _projectiles.clear();

    Mix_FreeChunk(_shot_sound);
}

bool Player::init(const char *tex_path)
{
    _num_extra_shots = 20;

    _w = _h = _init_w = _init_h = 24;
    _shoot_timer.start();

    _shot_sound = Mix_LoadWAV("res/player_shoot_01.wav");
    if(_shot_sound == 0)
        return false;

    return true;
}

void Player::render()
{
    glPushMatrix();
        glTranslatef(_x + _w / 2, _y + _h / 2, 0.0f);
        glRotatef(_rot, 0.0f, 0.0f, 1.0f);
        glColor3f(1.0f, 1.0f, 1.0f);

        glBegin(GL_TRIANGLES);
            glVertex2i(      0,   -_h);
            glVertex2i( _w / 2,    0);
            glVertex2i(-_w / 2,    0);
        glEnd();
    glPopMatrix();

    for(unsigned int i = 0; i < _projectiles.size(); ++i)
    {
        _projectiles[i].render();
    }
}

void Player::update()
{
    if(_tier != _old_tier)
    {
        _w = _init_w * _tier;
        _h = _init_h * _tier;
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

    if(_speed != 0)
    {
        if((_x + _w / 2) > _cur_mouse_x - _w / 4 && (_x + _w / 4) < _cur_mouse_x + _w / 4 &&
           (_y + _h / 2) > _cur_mouse_y - _h / 4 && (_y + _h / 4) < _cur_mouse_y + _h / 4 && _mouse_control)
        {}
        else
        {
            _x += _stepx * _speed * timing::get_deltatime();
            _y += _stepy * _speed * timing::get_deltatime();
        }
    }

    if(_x + _w > _screen->w)
        _x = _screen->w - _w - 1;

    if(_x < 0)
        _x = 1;

    if(_y + _h + _h / 2 > _screen->h)
        _y = _screen->h - _h - 1 - _h / 2;

    if(_y - _h / 2 < 0)
        _y = 1 + _h / 2;

    if(_shooting && _shoot_timer.get_ticks() / 1000.0f > _shoot_wait)
    {
        this->shoot((float) _cur_mouse_x, (float) _cur_mouse_y);
        _shoot_timer.start();
    }
}

void Player::shoot(float mouse_x, float mouse_y)
{
    Mix_PlayChannel(-1, _shot_sound, 0);

    float x = (_x + _w / 2) - mouse_x;
    float y = (_y + _h / 2) - mouse_y;

    float len = sqrt(x*x + y*y);
    float stepx = -x / len;
    float stepy = -y / len;

    // std::cout << "Shooting projectile with direction (" << stepx << ", " << stepy << ")" << std::endl;

    Projectile p;
    p.x = (_x + _w / 2);
    p.y = (_y + _h / 2);
    p.stepx = stepx;
    p.stepy = stepy;
    p.speed = 750 / (_tier) + _extra_proj_speed;

    p.col_r = 1.0f - 1.0f / 0.0f;
    p.col_g = 1.0f - 1.0f / 255;
    p.col_b = 1.0f - 1.0f / 0.0f;

    _projectiles.push_back(p);
    p.parent_index = _projectiles.size() - 1;

    if(_num_extra_shots > 0)
    {
        Projectile p2(p);
        p2.x -= 9;
        p2.y -= 9;
        _projectiles.push_back(p2);
        p2.parent_index = _projectiles.size() - 1;

        Projectile p3(p);
        p3.x += 9;
        p3.y += 9;
        _projectiles.push_back(p3);
        p3.parent_index = _projectiles.size() - 1;
        --_num_extra_shots;
    }
}

void Player::update_rotation(float mouse_x, float mouse_y)
{
    int dx = mouse_x - (_x + _w / 2);
    int dy = (_y + _h / 2) - mouse_y;
    _rot = atan2(-dy, dx) * 180 / PI + 90;

    int x = (_x + _w / 2) - mouse_x;
    int y = (_y + _h / 2) - mouse_y;

    float len = sqrt(x*x + y*y);
    _stepx = -x / len;
    _stepy = -y / len;

    _cur_mouse_x = mouse_x;
    _cur_mouse_y = mouse_y;
}

void Player::on_mouse_event(SDL_Event *ev)
{
    if(ev->type == SDL_MOUSEBUTTONDOWN)
    {
        if(ev->button.button == SDL_BUTTON_LEFT)
        {
            if(_shoot_timer.get_ticks() / 1000.0f > _shoot_wait)
            {
                this->shoot((float) ev->button.x, (float) ev->button.y);
                _shoot_timer.start();
            }

            _shooting = true;
        }
    }
    else if(ev->type == SDL_MOUSEBUTTONUP)
    {
        if(ev->button.button == SDL_BUTTON_LEFT)
        {
            _shooting = false;
        }
    }
}

void Player::on_key_event(SDL_Event *ev)
{
    if(ev->type == SDL_KEYDOWN)
    {
        if(ev->key.keysym.sym == SDLK_DOWN || ev->key.keysym.sym == SDLK_s)
        {
            _speed = -300 + _extra_speed;
        }
        else if(ev->key.keysym.sym == SDLK_UP || ev->key.keysym.sym == SDLK_w)
        {
            _speed = 300 + _extra_speed;
        }
    }
    else if(ev->type == SDL_KEYUP)
    {
        if(ev->key.keysym.sym == SDLK_DOWN || ev->key.keysym.sym == SDLK_s)
        {
            _speed = 0;
        }
        else if(ev->key.keysym.sym == SDLK_UP || ev->key.keysym.sym == SDLK_w)
        {
            _speed = 0;
        }
    }
}
