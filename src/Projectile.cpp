#include "Projectile.hpp"

void Projectile::render()
{
    glColor3f(col_r, col_g, col_b);

    glBegin(GL_QUADS);
        glVertex2f(x, y);
        glVertex2f(x + w, y);
        glVertex2f(x + w, y + h);
        glVertex2f(x, y + h);
    glEnd();
}
