#ifndef _APPLICATION_HPP_
#define _APPLICATION_HPP_

#include "project_config.hpp"
#include "window.hpp"
#include <unordered_map>
#include <exception>
#include <atomic>
#include <list>

namespace zm {

	class Application {

		public :

		Application ( ) {

			/* make sure we are doing it only once */
			if(!(this)->was_initialized) {

				if(SDL_Init(SDL_INIT_EVERYTHING) < 0) {

					throw std::logic_error("SDL_Init failed");
				}

				Event::type_pool = SDL_RegisterEvents(256);

				if(Event::type_pool == -1) {

					SDL_Quit();

					throw std::logic_error("SDL_RegisterEvents failed");
				}

				if(IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_TIF | IMG_INIT_WEBP) == 0) {

					SDL_Quit();

					throw std::logic_error("IMG_Init failed");
				}

				if(TTF_Init() == -1) {

					IMG_Quit();

					SDL_Quit();

					throw std::logic_error("TTF_Init failed");
				}

				if(SDLNet_Init() == -1) {

					IMG_Quit();

					SDL_Quit();

					TTF_Quit();

					throw std::logic_error("SDLNet_Init failed");
				}

				(this)->running = true;

				(this)->was_initialized = true;

				SDL_EventState(SDL_DROPFILE,SDL_ENABLE);
			}

			(this)->application_count = (this)->application_count + 1;
		}

		~ Application ( ) {

			/* make sure we are doing it only once */
			if((this)->was_initialized) {

				SDLNet_Quit();

				TTF_Quit();

				IMG_Quit();

				SDL_Quit();
			}

			(this)->application_count = (this)->application_count - 1;
		}

		static Window * create_window ( char const * const title, int32_t const & x, int32_t const & y, int32_t const & width, int32_t const & height, uint32_t const & flags, Window * parent, Window::callback_t const callback ) {

			if(callback == nullptr) {

				return nullptr;
			}

			Window * window = new Window;

			/* assign callback function */
			window->callback = callback;

			/* create SDL_Window, force OpenGL */
			window->handle = SDL_CreateWindow(title,x,y,width,height,flags | SDL_WINDOW_OPENGL);

			if(window->handle == nullptr) {

				delete window;

				return nullptr;
			}

			/* create SDL_GLContext */
			window->context = SDL_GL_CreateContext(window->handle);

			if(window->context == nullptr) {

				SDL_DestroyWindow(window->handle);

				delete window;

				return nullptr;
			}

			/* get window ID */
			window->id = SDL_GetWindowID(window->handle);

			/* register window */
			window_id_map[window->id] = window;

			/* increment window count */
			window->count++;

			if(parent != nullptr) {

				window->parent_window = parent;

				parent->child_windows.push_back(window);

				window->this_in_parent_list = --(parent->child_windows.end());

				Event event;

				event.type = ZM_WINDOW_EVENT;

				event.zm_event.window.event = ZM_WINDOW_EVENT_CHILD_CREATED;

				event.zm_event.window.window_id = parent->id;

				event.zm_event.window.prm1 = window->id;

				event.zm_event.window.ptr_prm = window;

				SDL_PushEvent(&event.sdl_event);
			}

			/* make sure we are doing it only once */
			if(window->count == 1) {

				uint32_t glew_error = glewInit();

				if( glew_error != GLEW_OK) {

					SDL_Quit();

					throw std::runtime_error((std::string("glewInit failed : ") + std::string((char*)glewGetErrorString(glew_error))).c_str());
				}
			}

			return window;
		}

		static void destroy_window ( Window * window ) {

			if(window == nullptr)

				return;

			auto it = window->child_windows.begin();

			auto it_cpy = it;

			Event destroy_event;

			destroy_event.zm_event.type = ZM_WINDOW_EVENT;

			destroy_event.zm_event.window.event = ZM_WINDOW_EVENT_DESTROY;

			/* destroy all child windows */
			for(; it != window->child_windows.end(); it = it_cpy) {

				it_cpy++;

				/* call callback function with ZM_WINDOW_EVENT_DESTROY */
				(*it)->callback(*it,destroy_event);

				destroy_window(*it);
			}

			/* call callback function with ZM_WINDOW_EVENT_DESTROY */
			window->callback(window,destroy_event);

			if(window->parent_window != nullptr) {

				/* remove self from parent child list */
				window->parent_window->child_windows.erase(window->this_in_parent_list);

				/* post ZM_WINDOW_CHILD_DESTROYED on event queue */
				Event event;

				event.type = ZM_WINDOW_EVENT;

				event.zm_event.window.event = ZM_WINDOW_EVENT_CHILD_DESTROYED;

				event.zm_event.window.window_id = window->parent_window->id;

				event.zm_event.window.prm1 = window->id;

				event.zm_event.window.ptr_prm = nullptr;

				SDL_PushEvent(&event.sdl_event);
			}

			/* find window */
			auto map_it = window_id_map.find(window->id);

			/* unregister window */
			if(map_it != window_id_map.end())

				window_id_map.erase(map_it);

			/* SDL clean up */
			SDL_GL_DeleteContext(window->context);

			SDL_DestroyWindow(window->handle);

			window->count = window->count - 1;

			/* when all windos are closed SDL also closes its video subsystem, meaning that
			you can no longer create new windows unless you reinitialize the video subsytem */

			/* if this is the last application ref */
			if(application_count == 1)

				/* if there are no more windows */
				if(window->count == 0)

					running = false;

			delete window;
		}

		static bool is_running ( ) {

			return running;
		}

		static int32_t next_event ( Event & event ) {

			return SDL_PollEvent(&event.sdl_event);
		}

		static int32_t read_event ( Event & event ) {

			return SDL_PeepEvents(&event.sdl_event,1,SDL_GETEVENT,0,0);
		}

		static int32_t peek_event ( Event & event ) {

			return SDL_PeepEvents(&event.sdl_event,1,SDL_PEEKEVENT,0,0);
		}

		static int32_t process_event ( Event const & event ) {

			auto it = window_id_map.find(event.sdl_event.window.windowID);

			if(it == window_id_map.end())

				return -1;

			return (it)->second->callback((it)->second,event);
		}

		static bool is_open_window ( Window const * const window ) {

			if(window == nullptr)

				return false;

			auto it = window_id_map.find(window->id);

			if(it != window_id_map.end())

				return true;

			return false;
		}

		private :

		static std::unordered_map<uint32_t,Window *> window_id_map;

		static uint32_t application_count;

		static bool running;

		static bool was_initialized;
	};

	std::unordered_map<uint32_t,Window *> Application::window_id_map;

	uint32_t Application::application_count = 0;

	bool Application::running = false;

	bool Application::was_initialized = false;

} /* namespace zm */

#endif /* #ifndef _APPLICATION_HPP_ */