/*
 *	project_config.hpp
 */

#ifndef _PROJECT_CONFIG_HPP_
#define _PROJECT_CONFIG_HPP_

#ifdef USE_WINDOWS

/* disable few annoying warnings */
#define _CRT_SECURE_NO_WARNINGS 1

/* include all third party dependency header files */
#include "Windows.h"
#include "D:/SDL 2.0.1/include/SDL.h"
#include "D:/SDL2_image-2.0.0/include/SDL_image.h"
#include "D:/SDL2_ttf-2.0.12/include/SDL_ttf.h"
#include "D:/SDL2_net-2.0.0/include/SDL_net.h"
#include "D:/glew-1.11.0/include/GL/glew.h"
#include "GL/gl.h"

/* link libraries */
#pragma comment(lib,"D:/SDL 2.0.1/lib/x86/SDL2.lib")
#pragma comment(lib,"D:/SDL2_image-2.0.0/lib/x86/SDL2_image.lib")
#pragma comment(lib,"D:/SDL2_ttf-2.0.12/lib/x86/SDL2_ttf.lib")
#pragma comment(lib,"D:/SDL2_net-2.0.0/lib/x86/SDL2_net.lib")
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

#define _THREAD_LOCAL_ __declspec(thread)
#define _THREAD_ALLOC_ __declspec(thread)

#elif defined USE_LINUX
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_net.h>
#include <GL/glew.h>

#define _GET_UTC_TIME_MS_ SDL_GetTicks()
#endif

#endif /* #ifndef _PROJECT_CONFIG_HPP_ */