/*
 *	project_config.hpp
 */

#ifndef _PROJECT_CONFIG_HPP_
#define _PROJECT_CONFIG_HPP_

/* include all system specific header files */
#include "Windows.h"
#include "D:/SDL 2.0.1/include/SDL.h"
#include "D:/SDL2_image-2.0.0/include/SDL_image.h"
#include "GL/gl.h"

/* link libraries */
#pragma comment(lib,"D:/SDL 2.0.1/lib/x86/SDL2.lib")
#pragma comment(lib,"D:/SDL2_image-2.0.0/lib/x86/SDL2_image.lib")
#pragma comment(lib,"opengl32.lib")

/* undef main if defined */
#ifdef main
#undef main
#endif

#define _STRONG_INLINE_ __forceinline
#define _ALIGN(X)		__declspec(align(X))

#ifdef _BIG_ENDIAN_

#else

template < typename type > type adjust_endianess ( void const * const ptr ) {

	return ((type*)ptr)[0];
}

#endif


#endif /* #ifndef _PROJECT_CONFIG_HPP_ */