#include "zm.hpp"
#include <iostream>

int32_t main_callback ( zm::Window * window, zm::Event const & event ) {

	if(event.type == SDL_WINDOWEVENT)

		if(event.sdl_event.window.event == SDL_WINDOWEVENT_CLOSE)

			zm::Application::destroy_window(window);

	return 0;
}

int main ( int argc, char ** argv ) {

	zm::Application * application = nullptr;

	try {

		/* initialize application */
		application = new zm::Application();
	}

	/* if initialization failed */
	catch (std::exception & e) {

		delete application;

		application = nullptr;

		throw std::runtime_error(e.what());
	}

	zm::Window * main_window = nullptr;

	try {

		/* might throw an exception when creating the very first window */
		main_window = application->create_window("Monstrosity",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,1248,800,SDL_WINDOW_RESIZABLE,nullptr,main_callback);
	}

	catch(std::exception & e) {

		throw std::runtime_error(e.what());
	}

	if(main_window == nullptr) {

		delete application;

		return 0;
	}

	zm::Animation anim;

	anim.generate_frames(zm::Vector2U(480,480),zm::Vector2U(0,0),zm::Vector2U(61,70),zm::Vector2U(0,0),zm::Vector2U(7,4),27);

	anim.run_time = 1000;

	/* lounch animation editor */
	anim.edit(main_window);

	zm::Event event;

	while(application->is_running()) {

		if(application->next_event(event))

			application->process_event(event);
	}

	delete application;

	return 0;
}