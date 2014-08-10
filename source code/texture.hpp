#ifndef _TEXTURE_HPP_
#define _TEXTURE_HPP_

#include "project_config.hpp"
#include <string>

namespace zm {

	GLuint load_texture ( std::string const & filename ) {

		SDL_Surface * surface = IMG_Load(filename.c_str());

		if(!surface) {

			return 0;
		}

		GLenum texture_format;

		GLint bytesPerPixel;

		bytesPerPixel = surface->format->BytesPerPixel;

		if(bytesPerPixel == 4) {

			if(surface->format->Rmask == 0x000000ff) {

				texture_format = GL_RGBA;
			}

			else {

				texture_format = GL_BGRA_EXT;
			}
		}

		else if(bytesPerPixel == 3) {

			if(surface->format->Rmask == 0x000000ff) {

				texture_format = GL_RGB;
			}

			else {

				texture_format = GL_BGR_EXT;
			}
		}

		else {

			SDL_FreeSurface(surface);

			return 0;
		}

		GLuint tex;

		glGenTextures(1, &tex);

		glBindTexture(GL_TEXTURE_2D, tex);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTexImage2D(GL_TEXTURE_2D, 0, bytesPerPixel, surface->w, surface->h, 0, texture_format, GL_UNSIGNED_BYTE, surface->pixels);

		SDL_FreeSurface(surface);

		return tex;
	}

	void bind_texture ( GLuint const & texture_id, GLenum const & target = GL_TEXTURE_2D ) {

		glBindTexture(target,texture_id);
	}

} /* namespace zm */ 

#endif /* #ifndef _TEXTURE_HPP_ */