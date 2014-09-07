#ifndef _EVENT_HPP_
#define _EVENT_HPP_

#include "project_config.hpp"

namespace zm {

	struct ZM_window_event {

		uint32_t type;

		uint32_t timestamp;

		uint32_t window_id;

		uint32_t event;

		uint32_t prm1;

		uint32_t prm2;

		void * ptr_prm;
	};

	struct ZM_Event {

		union {

			uint32_t type;
			ZM_window_event	window;
		};
	};

	struct Event {

		union {

			uint32_t type;
			SDL_Event sdl_event;
			ZM_Event zm_event;
		};

		static uint32_t type_pool;
	};

	uint32_t Event::type_pool;


#define _ZM_WINDOW_EVENT_CODE_				0x01

#define ZM_WINDOW_EVENT						::zm::Event::type_pool + _ZM_WINDOW_EVENT_CODE_

#define ZM_WINDOW_EVENT_DESTROY				0x01

#define ZM_WINDOW_EVENT_CHILD_CREATED		0x02
#define ZM_WINDOW_EVENT_CHILD_DESTROYED		0x03

#define ZM_WINDOW_EVENT_RENDER				0x04


} /* namespace zm */

#endif /* #ifndef _EVENT_HPP_ */