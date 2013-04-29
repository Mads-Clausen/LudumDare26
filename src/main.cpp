#include <iostream>
#include <vector>
#include <time.h>
#include <cstdlib>
#include <string>
#include <sstream>
#include <sys/resource.h>

#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_opengl.h>
#include <SDL/SDL_ttf.h>

#include "Render.hpp"
#include "Player.hpp"
#include "Enemy.hpp"
#include "Timing.hpp"
#include "Projectile.hpp"
#include "Particles.hpp"
#include "Powerup.hpp"

int S_WIDTH = 800, S_HEIGHT = 600, S_FULL_W = 0, S_FULL_H = 0;
SDL_Surface *screen;
SDL_VideoInfo *info;

float current_enemy_tier = 1.0f;

Player player;
std::vector<Enemy> enemies;

bool running = true, ingame = true, game_over = false, paused = false;

Mix_Chunk *firework_sound;

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
int enemy_count = 0, player_h_c = 0;
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

        if(player_h_c >= 2)
        {
            player.set_health(player.get_health() + 1);
            player_h_c = -1;
        }

        ++player_h_c;

        enemy_count = 0;
    }
}

void kill_enemy(int i)
{
    enemies.erase(enemies.begin() + i);
}

void restart()
{
    player.~Player();
    player = *(new Player());

    if(!player.init("res/player_01.bmp"))
    {
        std::cout << "Unable to initialise player: " << Mix_GetError() << std::endl;
        running = false;
    }

    enemies.clear();
    spawn_enemy();
    spawn_timer.start();
    running = true;
    paused = game_over = false;
    ingame = true;
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
    screen = SDL_SetVideoMode(S_WIDTH, S_HEIGHT, 32, SDL_SWSURFACE | SDL_OPENGL | SDL_RESIZABLE);

    if(screen == 0)
    {
        std::cout << "Unable to initialise window" << std::endl;
        running = false;
    }

    Drawable::_screen = screen;

    std::cout << "Initialising sound module..." << std::endl;
    if(Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096) == -1)
    {
        std::cout << "Unable to initialise SDL_Mixer" << std::endl;
        running = false;
    }

    firework_sound = Mix_LoadWAV("res/end_fireworks.wav");
    if(firework_sound == 0)
    {
        std::cout << "Unable to load sound: " << Mix_GetError() << std::endl;
        running = false;
    }

    std::cout << "Initialising OpenGL..." << std::endl;
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

    int w, h;
    GLuint gameover_text = create_gl_surface("res/gameover_text.bmp", w, h);
    if(w < 0 || h < 0)
    {
        std::cout << "Unable to load image." << std::endl;
        running = false;
    }

    GLuint paused_text = create_gl_surface("res/paused_text.bmp", w, h);
    if(w < 0 || h < 0)
    {
        std::cout << "Unable to load image." << std::endl;
        running = false;
    }


    timing::Timer firework_timer;
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
                if(ingame) player.update_rotation((float) cur_mouse_x, (float) cur_mouse_y);
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
                    if(ingame)
                    {
                        ingame = false;
                        paused = true;
                    }
                    else
                    {
                        if(paused)
                        {
                            paused = game_over = false;
                            ingame = true;
                        }
                    }
                }
                else if(event.key.keysym.sym == SDLK_RETURN)
                {
                    if(!ingame && game_over)
                    {
                        restart();
                    }
                    else if(!ingame && paused && !game_over)
                    {
                        paused = false;
                        ingame = true;
                    }
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
            PowerupManager::update();

            if(spawn_timer.get_ticks() / 1000.0f > 4.0f / (current_enemy_tier - 0.2))
            {
                spawn_enemy();
                spawn_timer.start();
            }

            player.update();
            if(player.is_dead())
            {
                firework_timer.start();
                ingame = false;
                game_over = true;
                paused = false;
            }

            // Update shitty collision and stuffs
            std::vector<Projectile>& proj = player.get_projectiles();
            for(unsigned int i = 0; i < enemies.size(); ++i)
            {
                enemies[i].update();

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

            std::vector<Powerup*>& pow = PowerupManager::get_powerups();
            int px = player.get_x() + player.get_w() / 2;
            int py = player.get_y() + player.get_h() / 2;
            for(unsigned int i = 0; i < pow.size(); ++i)
            {

                if(px > pow[i]->x && px < pow[i]->x + pow[i]->w &&
                   py > pow[i]->y && py < pow[i]->y + pow[i]->h)
                {
                    player.add_powerup(pow[i]->type, pow[i]->d);
                    pow[i]->kill();
                }
            }
        }


        ParticleManager::update();
        ParticleManager::render();

        PowerupManager::render();
        for(unsigned int i = 0; i < enemies.size(); ++i)
        {
            enemies[i].render();
        }

        player.render();

        if(!ingame && game_over)
        {
            if(firework_timer.get_ticks() / 1000.0f > (((double) rand()) / RAND_MAX) * 3.0f + (((double) rand()) / RAND_MAX) * 1.5f)
            {
                if(Mix_PlayChannel(-1, firework_sound, 0) == -1)
                {
                    std::cout << "Unable to play sound: " << Mix_GetError() << std::endl;
                }

                float r = (((double) rand()) / RAND_MAX);
                float g = (((double) rand()) / RAND_MAX);
                float b = (((double) rand()) / RAND_MAX);
                ParticleSystem *psys = new ParticleSystem((((double) rand()) / RAND_MAX) * (S_WIDTH - 100) + 50, (((double) rand()) / RAND_MAX) * (S_HEIGHT - 100 - 200) + 50, 40, 0.001f, 0, 360, 100, r, g, b, 400, 2, true);
                psys->start();
                ParticleManager::register_system(psys);
                firework_timer.start();
            }

            glBindTexture(GL_TEXTURE_2D, gameover_text);
            glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

            glBegin(GL_QUADS);
                glTexCoord2i(0, 0);
                glVertex2i(S_WIDTH / 2 - 256, S_HEIGHT - 256);

                glTexCoord2i(1, 0);
                glVertex2i(S_WIDTH / 2 + 256, S_HEIGHT - 256);

                glTexCoord2i(1, 1);
                glVertex2i(S_WIDTH / 2 + 256, S_HEIGHT);

                glTexCoord2i(0, 1);
                glVertex2i(S_WIDTH / 2 - 256, S_HEIGHT);
            glEnd();

            glBindTexture(GL_TEXTURE_2D, 0);
        }
        else if(!ingame && paused)
        {
            glBindTexture(GL_TEXTURE_2D, paused_text);
            glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

            glBegin(GL_QUADS);
                glTexCoord2i(0, 0);
                glVertex2i(S_WIDTH / 2 - 256, S_HEIGHT - 256);

                glTexCoord2i(1, 0);
                glVertex2i(S_WIDTH / 2 + 256, S_HEIGHT - 256);

                glTexCoord2i(1, 1);
                glVertex2i(S_WIDTH / 2 + 256, S_HEIGHT);

                glTexCoord2i(0, 1);
                glVertex2i(S_WIDTH / 2 - 256, S_HEIGHT);
            glEnd();

            glBindTexture(GL_TEXTURE_2D, 0);
        }

        std::stringstream ss;
        ss << "Ludum Dare #26 - FPS: " << (int) timing::get_fps();
        SDL_WM_SetCaption(ss.str().c_str(), 0);

        SDL_GL_SwapBuffers();
    }

    SDL_FreeSurface(screen);
    Mix_Quit();
    SDL_Quit();
    return 0;
}
