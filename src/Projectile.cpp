#include "Projectile.hpp"

void Projectile::render()
{
    glColor4f(col_r, col_g, col_b, 1.0f);

    glBegin(GL_LINES);
        glVertex2f(x, y);
        glVertex2f(x + stepx * 8, y + stepy * 8);
        // glVertex2f(x + w + stepx * 8, y + h + stepy * 8);
        // glVertex2f(x + stepx * 8, y + h + stepy * 8);
    glEnd();
}
