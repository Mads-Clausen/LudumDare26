#include "Powerup.hpp"

void Powerup::render()
{
    glPushMatrix();
        glColor4f(1.0f, 1.0f, 1.0f, _a);

        glBindTexture(GL_TEXTURE_2D, _tex);
        glBegin(GL_QUADS);
            glTexCoord2i(0, 0);
            glVertex2i(x, y);

            glTexCoord2i(1, 0);
            glVertex2i(x + w, y);

            glTexCoord2i(1, 1);
            glVertex2i(x + w, y + h);

            glTexCoord2i(0, 1);
            glVertex2i(x, y + h);
        glEnd();

        glColor3f(col_r, col_g, col_b);
        draw_circle(0.0f, 0.0f, w / 2 + 2, 180);
    glPopMatrix();
}

std::vector<Powerup*> PowerupManager::_reg;

void PowerupManager::register_powerup(Powerup *p)
{
    _reg.push_back(p);
}

void PowerupManager::render()
{
    for(unsigned int i = 0; i < _reg.size(); ++i)
    {
        _reg[i]->render();
    }
}

void PowerupManager::update()
{
    for(unsigned int i = 0; i < _reg.size(); ++i)
    {
        _reg[i]->update();

        if(_reg[i]->is_dead())
        {
            delete _reg[i];
            _reg.erase(_reg.begin() + i--);
        }
    }
}

std::vector<Powerup*>& PowerupManager::get_powerups()
{
    return _reg;
}
