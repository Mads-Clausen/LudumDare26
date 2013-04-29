#include "Render.hpp"

SDL_Surface *Drawable::_screen;


// COPIED FROM THE INTERWUBS, http://content.gpwiki.org/index.php/C%3aSDL_OGL
GLuint create_gl_surface(const char *tex_path, int &w, int &h)
{
    GLuint texture;			// This is a handle to our texture object
    SDL_Surface *surface;	// This surface will tell us the details of the image
    GLenum texture_format = GL_RGB;
    GLint  nOfColors;

    if ((surface = SDL_LoadBMP(tex_path))) {

        w = surface->w; h = surface->h;

        // Check that the image's width is a power of 2
        if((surface->w & (surface->w - 1)) != 0) {
            printf("warning: image's width is not a power of 2\n");
            surface->w = surface->h = -1;
            return -1;
        }

        // Also check if the height is a power of 2
        if((surface->h & (surface->h - 1)) != 0) {
            printf("warning: image's height is not a power of 2\n");
            surface->w = surface->h = -1;
            return -1;
        }

            // get the number of channels in the SDL surface
            nOfColors = surface->format->BytesPerPixel;
            if(nOfColors == 4)     // contains an alpha channel
            {
                    if(surface->format->Rmask == 0x000000ff)
                            texture_format = GL_RGBA;
                    else
                            texture_format = GL_BGRA;
            }
            else if(nOfColors == 3)     // no alpha channel
            {
                    if (surface->format->Rmask == 0x000000ff)
                            texture_format = GL_RGB;
                    else
                            texture_format = GL_BGR;
            }
            else
            {
                    printf("warning: the image is not truecolor..  this will probably break\n");
                    // this error should not go unhandled
            }

        // Have OpenGL generate a texture object handle for us
        glGenTextures( 1, &texture);

        // Bind the texture object
        glBindTexture( GL_TEXTURE_2D, texture);

        // Set the texture's stretching properties
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Edit the texture object's image data using the information SDL_Surface gives us
        glTexImage2D(GL_TEXTURE_2D, 0, nOfColors, surface->w, surface->h, 0, texture_format, GL_UNSIGNED_BYTE, surface->pixels);
    }
    else {
        printf("SDL could not load image.bmp: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Free the SDL_Surface only if it was successfully created
    if(surface) {
        SDL_FreeSurface(surface);
    }

    return texture;
}

void draw_circle(float cx, float cy, float r, int num_segments)
{
	glBegin(GL_LINE_LOOP);
	for(int ii = 0; ii < num_segments; ii++)
	{
		float theta = 2.0f * PI * float(ii) / float(num_segments);

		float x = r * cosf(theta);
		float y = r * sinf(theta);

		glVertex2f(x + cx, y + cy);
	}
	glEnd();
}
