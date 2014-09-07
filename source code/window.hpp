#ifndef _WINDOW_HPP_
#define _WINDOW_HPP_

#include "project_config.hpp"
#include "lin_alg.hpp"
#include "event.hpp"
#include <atomic>
#include <list>

namespace zm {

	class Window {

		public :

		typedef int32_t (* callback_t)(Window *, Event const &);

		Window ( ) {

			(this)->handle = nullptr;

			(this)->context = nullptr;
		
			(this)->callback = nullptr;

			(this)->parent_window = nullptr;
		}

		uint32_t get_flags ( ) const {
			
			return SDL_GetWindowFlags((this)->handle); 
		}

		void set_title ( std::string const & title ) const {
			
			SDL_SetWindowTitle((this)->handle,title.c_str()); 
		}

		std::string get_title ( ) const { 

			char const * const ptr = SDL_GetWindowTitle((this)->handle);

			std::string title(ptr);

			/* delete ptr? */

			return title;
		}

		void set_icon ( SDL_Surface * icon ) const {

			SDL_SetWindowIcon((this)->handle,icon);
		}

		void set_data ( std::string const & name, void * data ) const {

			SDL_SetWindowData((this)->handle,name.c_str(),data);
		}

		void * get_data ( std::string const & name ) const {

			return SDL_GetWindowData((this)->handle,name.c_str());
		}

		void set_position ( Vector<2,int32_t> const & pos ) const {

			SDL_SetWindowPosition((this)->handle,pos.x,pos.y);
		}

		Vector<2,int32_t> get_position ( ) const {

			Vector<2,int32_t> pos;

			SDL_GetWindowPosition((this)->handle,&pos.x,&pos.y);

			return pos;
		}

		void set_size ( Vector<2,int32_t> const & size ) const {

			SDL_SetWindowSize((this)->handle,size.x,size.y);
		}

		Vector<2,int32_t> get_size ( ) const {

			Vector<2,int32_t> size;

			SDL_GetWindowSize((this)->handle,&size.x,&size.y);

			return size;
		}

		void set_minimum_size ( Vector<2,int32_t> const & size ) const {

			SDL_SetWindowMinimumSize((this)->handle,size.x,size.y);
		}

		Vector<2,int32_t> get_minimum_size ( ) const {

			Vector<2,int32_t> size;

			SDL_GetWindowMinimumSize((this)->handle,&size.x,&size.y);

			return size;
		}

		void set_maximum_size ( Vector<2,int32_t> const & size ) const {

			SDL_SetWindowMaximumSize((this)->handle,size.x,size.y);
		}

		Vector<2,int32_t> get_maximum_size ( ) const {

			Vector<2,int32_t> size;

			SDL_GetWindowMaximumSize((this)->handle,&size.x,&size.y);

			return size;
		}

		void set_border ( bool const & border ) const {

			if(border)

				SDL_SetWindowBordered((this)->handle,SDL_bool::SDL_TRUE);

			SDL_SetWindowBordered((this)->handle,SDL_bool::SDL_FALSE);
		}

		void show ( ) const {

			SDL_ShowWindow((this)->handle);
		}

		void hide ( ) const {

			SDL_HideWindow((this)->handle);
		}

		void raise ( ) const {

			SDL_RaiseWindow((this)->handle);
		}

		void maximize ( ) const {

			SDL_MaximizeWindow((this)->handle);
		}

		void minimize ( ) const {

			SDL_MinimizeWindow((this)->handle);
		}

		void restore ( ) const {

			SDL_RestoreWindow((this)->handle);
		}

		void set_fulscreen ( uint32_t const & flags ) const {

			SDL_SetWindowFullscreen((this)->handle,flags);
		}

		void set_grab ( bool const & grab ) const {

			if(grab)

				SDL_SetWindowGrab((this)->handle,SDL_bool::SDL_TRUE);

			SDL_SetWindowGrab((this)->handle,SDL_bool::SDL_FALSE);
		}

		bool get_grab ( ) const {

			if(SDL_GetWindowGrab((this)->handle) == SDL_bool::SDL_TRUE)

				return true;

			return false;
		}

		bool set_brightness ( float const & brightness ) const {

			return !SDL_SetWindowBrightness((this)->handle,brightness);
		}

		float get_brightness ( ) const {

			return SDL_GetWindowBrightness((this)->handle);
		}

		bool set_gamma_ramp ( Vector<3,uint16_t> const & color ) const {

			return !SDL_SetWindowGammaRamp((this)->handle,&color.r,&color.g,&color.b);
		}

		bool get_gamma_ramp ( Vector<3,uint16_t> & color ) const {

			return !SDL_GetWindowGammaRamp((this)->handle,&color.r,&color.g,&color.b);
		}

		void make_current ( ) const {

			SDL_GL_MakeCurrent((this)->handle,(this)->context);
		}

		static bool is_screen_saver_enabled ( ) {

			if(SDL_IsScreenSaverEnabled() == SDL_bool::SDL_TRUE)

				return true;

			return false;
		}

		static void enable_screen_saver ( ) {

			SDL_EnableScreenSaver();
		}

		static void disable_screen_saver ( ) {

			SDL_DisableScreenSaver();
		}

		uint32_t id;

		SDL_Window * handle;

		SDL_GLContext context;

		callback_t callback;

		Window * parent_window;
		
		std::list<Window*>::iterator this_in_parent_list;

		std::list<Window*> child_windows;

		static uint32_t count;
	};

	uint32_t Window::count = 0;

} /* namespace zm */

#endif /* #ifndef _WINDOW_HPP_ */