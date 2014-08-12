#ifndef _TEXTURE_HPP_
#define _TEXTURE_HPP_

#include "project_config.hpp"
#include "lin_alg.hpp"
#include <string>

namespace zm {

	bool get_surface_format ( SDL_Surface const * surface, GLint & iformat, GLenum & pformat ) {

		iformat = surface->format->BytesPerPixel;

		if(iformat == 4) {

			if(surface->format->Rmask == 0x000000ff) {

				pformat = GL_RGBA;

				return true;
			}

			else {

				pformat = GL_BGRA_EXT;

				return true;
			}
		}

		else if(iformat == 3) {

			if(surface->format->Rmask == 0x000000ff) {

				pformat = GL_RGB;

				return true;
			}

			else {

				pformat = GL_BGR_EXT;

				return true;
			}
		}
	
		return false;
	}

	GLuint load_texture ( std::string const & filename ) {

		SDL_Surface * surface = IMG_Load(filename.c_str());

		if(!surface) {

			return 0;
		}

		GLenum texture_format;

		GLint bytesPerPixel;

		if(!get_surface_format(surface,bytesPerPixel,texture_format)) {

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

	GLuint create_texture ( SDL_Surface const * const surface ) {

		if(!surface) {

			return 0;
		}

		GLenum texture_format;

		GLint bytesPerPixel;

			if(!get_surface_format(surface,bytesPerPixel,texture_format)) {

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

		return tex;
	}

	void update_texture ( GLuint const & old_texture, Vector2U const & new_wh, GLint const & new_iformat, GLenum const & new_pformat, void const * const new_pdata ) {

		glBindTexture(GL_TEXTURE_2D, old_texture);

		glTexImage2D(GL_TEXTURE_2D, 0, new_iformat, new_wh.w, new_wh.h, 0, new_pformat, GL_UNSIGNED_BYTE, new_pdata);
	};

	GLuint render_text_to_texture ( TTF_Font * const font, std::string const & str, SDL_Color const & fg, SDL_Color const & bg ) {

		SDL_Surface * text = TTF_RenderText_Shaded(font,str.c_str(),fg,bg);

		/* if something went wrong */
		if(!text)

			return 0;

		/* create texture out of SDL_Surface * */
		GLuint texture = create_texture(text);

		/* clean up */
		SDL_FreeSurface(text);

		return texture;
	};

	GLuint render_text_to_texture ( TTF_Font * const font, char const * const str, SDL_Color const & fg, SDL_Color const & bg ) {

		SDL_Surface * text = TTF_RenderUTF8_Shaded(font,str,fg,bg);

		/* if something went wrong */
		if(!text)

			return 0;

		/* create texture out of SDL_Surface * */
		GLuint texture = create_texture(text);

		/* clean up */
		SDL_FreeSurface(text);

		return texture;
	};

	GLuint render_text_to_texture ( TTF_Font * const font, std::wstring const & wstr, SDL_Color const & fg, SDL_Color const & bg ) {

		SDL_Surface * text = TTF_RenderUNICODE_Shaded(font,(uint16_t*)wstr.c_str(),fg,bg);

		/* if something went wrong */
		if(!text)

			return 0;

		/* create texture out of SDL_Surface * */
		GLuint texture = create_texture(text);

		/* clean up */
		SDL_FreeSurface(text);

		return texture;
	};

	GLuint render_text_update_texture ( TTF_Font * const font, std::string const & str, SDL_Color const & fg, SDL_Color const & bg, GLuint const & texture ) {

		SDL_Surface * text = TTF_RenderText_Shaded(font,str.c_str(),fg,bg);

		/* if something went wrong */
		if(!text)

			return 0;

		GLenum texture_format;

		GLint bytesPerPixel;

		if(!get_surface_format(text,bytesPerPixel,texture_format)) {

			SDL_FreeSurface(text);

			return 0;
		}

		update_texture(texture,Vector2U(text->w,text->h),bytesPerPixel,texture_format,text->pixels);

		/* clean up */
		SDL_FreeSurface(text);

		return texture;
	};

	GLuint render_text_update_texture ( TTF_Font * const font, char const * const str, SDL_Color const & fg, SDL_Color const & bg, GLuint const & texture ) {

		SDL_Surface * text = TTF_RenderUTF8_Shaded(font,str,fg,bg);

		/* if something went wrong */
		if(!text)

			return 0;

		GLenum texture_format;

		GLint bytesPerPixel;

		if(!get_surface_format(text,bytesPerPixel,texture_format)) {

			SDL_FreeSurface(text);

			return 0;
		}

		update_texture(texture,Vector2U(text->w,text->h),bytesPerPixel,texture_format,text->pixels);

		/* clean up */
		SDL_FreeSurface(text);

		return texture;
	};

	GLuint render_text_update_texture ( TTF_Font * const font, std::wstring const & wstr, SDL_Color const & fg, SDL_Color const & bg, GLuint const & texture ) {

		SDL_Surface * text = TTF_RenderUNICODE_Shaded(font,(uint16_t*)wstr.c_str(),fg,bg);

		/* if something went wrong */
		if(!text)

			return 0;

		GLenum texture_format;

		GLint bytesPerPixel;

		bytesPerPixel = text->format->BytesPerPixel;

		if(!get_surface_format(text,bytesPerPixel,texture_format)) {

			SDL_FreeSurface(text);

			return 0;
		}

		update_texture(texture,Vector2U(text->w,text->h),bytesPerPixel,texture_format,text->pixels);

		/* clean up */
		SDL_FreeSurface(text);

		return texture;
	};

	void bind_texture ( GLuint const & texture_id, GLenum const & target = GL_TEXTURE_2D ) {

		glBindTexture(target,texture_id);
	}

} /* namespace zm */ 

#endif /* #ifndef _TEXTURE_HPP_ */