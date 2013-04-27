#include "Player.hpp"

// float temp_x, temp_y;

Player::Player()
{
    _speed = 0;
    _mouse_control = true;
}

Player::~Player()
{
    for(unsigned int i = 0; i < _projectiles.size(); ++i)
    {
        delete _projectiles[i];
    }

    Mix_FreeChunk(_shot_sound);
}

bool Player::init(const char *tex_path)
{
    /*
    _tex = create_gl_surface(tex_path, _w, _h);
    if(_w == -1 || _h == -1)
    {
        printf("Unable to initialise player: failed to load texture.");
        return false;
    }

    printf("%i * %i\n", _w, _h);
    */
    _w = _h = 24;

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

        glBegin(GL_TRIANGLES);
            glColor3f(255.0f, 255.0f, 255.0f);
            glVertex2i(      0,   -_h);
            glVertex2i( _w / 2,    0);
            glVertex2i(-_w / 2,    0);
        glEnd();
    glPopMatrix();

    /*
    glBegin(GL_QUADS);
        glColor3f(1.0f, 0.0f, 0.0f);
        glVertex2f(temp_x, temp_y);
        glVertex2f(temp_x + 2, temp_y);
        glVertex2f(temp_x + 2, temp_y + 2);
        glVertex2f(temp_x, temp_y + 2);
    glEnd();
    //*/

    for(unsigned int i = 0; i < _projectiles.size(); ++i)
    {
        _projectiles[i]->render();
    }
}

void Player::update()
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

void Player::on_key_event(SDL_Event *ev)
{
    if(ev->type == SDL_KEYDOWN)
    {
        if(ev->key.keysym.sym == SDLK_DOWN || ev->key.keysym.sym == SDLK_s)
        {
            _speed = -300;
        }
        else if(ev->key.keysym.sym == SDLK_UP || ev->key.keysym.sym == SDLK_w)
        {
            _speed = 300;
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
