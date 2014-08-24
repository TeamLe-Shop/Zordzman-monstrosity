/*
 *	project_config.hpp
 */

#ifndef _PROJECT_CONFIG_HPP_
#define _PROJECT_CONFIG_HPP_

/* disable few annoying warnings */
#define _CRT_SECURE_NO_WARNINGS 1

/* include all third party dependency header files */
#include "Windows.h"
#include "D:/SDL 2.0.1/include/SDL.h"
#include "D:/SDL2_image-2.0.0/include/SDL_image.h"
#include "D:/SDL2_ttf-2.0.12/include/SDL_ttf.h"
#include "D:/glew-1.11.0/include/GL/glew.h"
#include "GL/gl.h"

/* link libraries */
#pragma comment(lib,"D:/SDL 2.0.1/lib/x86/SDL2.lib")
#pragma comment(lib,"D:/SDL2_image-2.0.0/lib/x86/SDL2_image.lib")
#pragma comment(lib,"D:/SDL2_ttf-2.0.12/lib/x86/SDL2_ttf.lib")
#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"D:/glew-1.11.0/lib/Release/Win32/glew32.lib")
#pragma comment(lib,"D:/glew-1.11.0/lib/Release/Win32/glew32s.lib")

/* undef main if defined */
#ifdef main
#undef main
#endif

#define _STRONG_INLINE_ __forceinline
#define _ALIGN(X)		__declspec(align(X))

#define _GET_UTC_TIME_MS_ SDL_GetTicks()

#endif /* #ifndef _PROJECT_CONFIG_HPP_ */