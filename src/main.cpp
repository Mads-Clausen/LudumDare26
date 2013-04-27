#include <iostream>
#include <vector>
#include <time.h>
#include <stdlib.h>
#include <string>
#include <sstream>

#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_opengl.h>

#include "Render.hpp"
#include "Player.hpp"
#include "Enemy.hpp"
#include "Timing.hpp"
#include "Projectile.hpp"

int S_WIDTH = 800, S_HEIGHT = 600, S_FULL_W = 0, S_FULL_H = 0;
SDL_Surface *screen;
SDL_VideoInfo *info;

Player player;
std::vector<Enemy> enemies;

bool running = true;

void init_gl()
{
    glEnable(GL_TEXTURE_2D);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glViewport(0, 0, S_WIDTH, S_HEIGHT);

    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glOrtho(0.0f, S_WIDTH, S_HEIGHT, 0.0f, -1.0f, 1.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void spawn_enemy()
{
    Enemy en;
    en.init("");
    en.set_target_player(&player);
    en.set_position(rand() % S_WIDTH + 1, rand() % S_HEIGHT + 1);
    enemies.push_back(en);
}

void kill_enemy(int i)
{
    enemies.erase(enemies.begin() + i);
}

int main()
{
    srand(time(0));

    std::cout << "Initialising SDL..." << std::endl;
    if(SDL_Init(SDL_INIT_EVERYTHING) == -1)
    {
        std::cout << "Unable to initialise SDL" << std::endl;
        running = false;
    }

    info = (SDL_VideoInfo*) SDL_GetVideoInfo();
    std::cout << "Screen resolution: " << info->current_w << "px * " << info->current_h << "px" << std::endl;
    S_FULL_W = info->current_w;
    S_FULL_H = info->current_h;

    std::cout << "Initialising window..." << std::endl;
    screen = SDL_SetVideoMode(S_WIDTH, S_HEIGHT, 32, SDL_SWSURFACE | SDL_OPENGL);

    if(screen == 0)
    {
        std::cout << "Unable to initialise window" << std::endl;
        running = false;
    }

    Drawable::_screen = screen;

    std::cout << "Initialising sound module..." << std::endl;
    if(Mix_OpenAudio( 22050, MIX_DEFAULT_FORMAT, 2, 4096 ) == -1)
    {
        std::cout << "Unable to initialise SDL_Mixer" << std::endl;
        running = false;
    }

    std::cout << "Initialising OpenGL..." << std::endl;
    init_gl();

    timing::init();

    if(!player.init("res/player_01.bmp"))
    {
        std::cout << "Unable to initialise player: " << Mix_GetError() << std::endl;
        running = false;
    }

    spawn_enemy();
    spawn_enemy();
    spawn_enemy();

    SDL_Event event;

    int cur_mouse_x, cur_mouse_y;
    while(running)
    {
        timing::update();

        while(SDL_PollEvent(&event))
        {
            if(event.type == SDL_QUIT)
            {
                running = false;
                std::cout << "Shutting down..." << std::endl;
                break;
            }
            else if(event.type == SDL_MOUSEMOTION)
            {
                cur_mouse_x = event.motion.x;
                cur_mouse_y = event.motion.y;
            }
            else if(event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
            {
                player.shoot((float) event.button.x, (float) event.button.y);
            }
            else if(event.type == SDL_KEYDOWN)
            {
                if(event.key.keysym.sym == SDLK_ESCAPE)
                {
                    running = false;
                    std::cout << "Shutting down..." << std::endl;
                    break;
                }
                else if(event.key.keysym.sym == SDLK_F11)
                {
                    if(screen->flags & SDL_FULLSCREEN)
                    {
                        SDL_FreeSurface(screen);
                        S_WIDTH = 800;
                        S_HEIGHT = 600;
                        init_gl();

                        screen = SDL_SetVideoMode(S_WIDTH, S_HEIGHT, 32, SDL_SWSURFACE | SDL_OPENGL);
                        Drawable::_screen = screen;
                    }
                    else
                    {
                        SDL_FreeSurface(screen);
                        S_WIDTH = S_FULL_W;
                        S_HEIGHT = S_FULL_H;
                        init_gl();

                        screen = SDL_SetVideoMode(S_WIDTH, S_HEIGHT, 32, SDL_FULLSCREEN | SDL_OPENGL);
                        Drawable::_screen = screen;
                    }
                }

                player.on_key_event(&event);
            }
            else if(event.type == SDL_KEYUP)
            {
                player.on_key_event(&event);
            }
        }

        if(!running)
            break;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        player.update();
        player.update_rotation((float) cur_mouse_x, (float) cur_mouse_y);

        // Update shitty collision and stuffs
        std::vector<Projectile*>& proj = player.get_projectiles();
        for(unsigned int i = 0; i < enemies.size(); ++i)
        {
            enemies[i].update();
            enemies[i].render();

            for(unsigned int x = 0; x < proj.size(); ++x)
            {
                if(enemies[i].check_collision(proj[x])) // weak enemy is deaaaad
                {
                    delete proj[x];
                    proj.erase(proj.begin() + x);

                    if(enemies[i].is_dead())
                    {
                        kill_enemy(i);
                        spawn_enemy();
                        break;
                    }
                }
            }
        }

        player.render();

        std::stringstream ss;
        ss << "Ludum Dare #26 - FPS:" << timing::get_fps();
        SDL_WM_SetCaption(ss.str().c_str(), 0);


        SDL_GL_SwapBuffers();
    }

    SDL_FreeSurface(screen);
    Mix_Quit();
    SDL_Quit();
    return 0;
}
