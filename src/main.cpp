#include <iostream>
#include <vector>
#include <time.h>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <sys/resource.h>

#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_opengl.h>

#include "Render.hpp"
#include "Player.hpp"
#include "Enemy.hpp"
#include "Timing.hpp"
#include "Projectile.hpp"
#include "Particles.hpp"

int S_WIDTH = 800, S_HEIGHT = 600, S_FULL_W = 0, S_FULL_H = 0;
SDL_Surface *screen;
SDL_VideoInfo *info;

float current_enemy_tier = 1.0f;

Player player;
std::vector<Enemy> enemies;

bool running = true, ingame = true;

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

timing::Timer spawn_timer;
int enemy_count = 0;
void spawn_enemy()
{
    Enemy en;
    en.init();
    en.set_target_player(&player);
    en.set_tier(current_enemy_tier);
    en.set_position(rand() % S_WIDTH + 1, rand() % S_HEIGHT + 1);
    enemies.push_back(en);

    ++enemy_count;
    if(enemy_count > 3)
    {
        current_enemy_tier += 0.1f;
        player.set_tier(player.get_tier() + 0.1f);
        enemy_count = 0;
    }
}

void kill_enemy(int i)
{
    enemies.erase(enemies.begin() + i);
}

int main()
{
    srand(time(0));

    std::cout << "Initialising \x1b[31;1mSDL\x1b[32;0m..." << std::endl;
    if(SDL_Init(SDL_INIT_EVERYTHING) == -1)
    {
        std::cout << "Unable to initialise SDL" << std::endl;
        running = false;
    }

    info = (SDL_VideoInfo*) SDL_GetVideoInfo();
    std::cout << "Screen resolution: " << info->current_w << "px * " << info->current_h << "px" << std::endl;
    S_FULL_W = info->current_w;
    S_FULL_H = info->current_h;

    std::cout << "Initialising \x1b[31;1mwindow\x1b[32;0m..." << std::endl;
    screen = SDL_SetVideoMode(S_WIDTH, S_HEIGHT, 32, SDL_SWSURFACE | SDL_OPENGL | SDL_RESIZABLE);

    if(screen == 0)
    {
        std::cout << "Unable to initialise window" << std::endl;
        running = false;
    }

    Drawable::_screen = screen;

    std::cout << "Initialising \x1b[31;1msound module\x1b[32;0m..." << std::endl;
    if(Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096) == -1)
    {
        std::cout << "Unable to initialise SDL_Mixer" << std::endl;
        running = false;
    }

    std::cout << "Initialising \x1b[31;1mOpenGL\x1b[32;0m..." << std::endl;
    init_gl();

    timing::init();
    spawn_timer.start();

    if(!player.init("res/player_01.bmp"))
    {
        std::cout << "Unable to initialise player: " << Mix_GetError() << std::endl;
        running = false;
    }

    SDL_Event event;

    spawn_enemy();

    int cur_mouse_x = 0, cur_mouse_y = 0;

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
                player.update_rotation((float) cur_mouse_x, (float) cur_mouse_y);
            }
            else if(event.type == SDL_MOUSEBUTTONDOWN)
            {
                if(ingame) player.on_mouse_event(&event);
            }
            else if(event.type == SDL_MOUSEBUTTONUP)
            {
                if(ingame) player.on_mouse_event(&event);
            }
            else if(event.type == SDL_KEYDOWN)
            {
                if(event.key.keysym.sym == SDLK_ESCAPE)
                {
                    running = false;
                    std::cout << "Shutting down..." << std::endl;
                    break;
                }

                if(ingame) player.on_key_event(&event);
            }
            else if(event.type == SDL_KEYUP)
            {
                if(ingame) player.on_key_event(&event);
            }
            else if(event.type == SDL_VIDEORESIZE)
            {
                S_WIDTH = event.resize.w;
                S_HEIGHT = event.resize.h;

                SDL_FreeSurface(screen);
                screen = SDL_SetVideoMode(S_WIDTH, S_HEIGHT, 32, SDL_SWSURFACE | SDL_OPENGL | SDL_RESIZABLE);
                init_gl();
            }
        }

        if(!running)
            break;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if(ingame)
        {
            if(spawn_timer.get_ticks() / 1000.0f > 4.0f / (current_enemy_tier - 0.2))
            {
                spawn_enemy();
                spawn_timer.start();
            }

            player.update();

            // Update shitty collision and stuffs
            std::vector<Projectile>& proj = player.get_projectiles();
            for(unsigned int i = 0; i < enemies.size(); ++i)
            {
                enemies[i].update();
                enemies[i].render();

                for(unsigned int x = 0; x < proj.size(); ++x)
                {
                    if(enemies[i].check_collision(&(proj[x]))) // weak enemy is deaaaad
                    {
                        proj.erase(proj.begin() + x);

                        if(enemies[i].is_dead())
                        {
                            kill_enemy(i);
                            break;
                        }
                    }
                }
            }

            player.render();
        }

        std::stringstream ss;
        ss << "Ludum Dare #26 - FPS: " << (int) timing::get_fps();
        SDL_WM_SetCaption(ss.str().c_str(), 0);

        ParticleManager::update();
        ParticleManager::render();

        SDL_GL_SwapBuffers();
    }

    SDL_FreeSurface(screen);
    Mix_Quit();
    SDL_Quit();
    return 0;
}
