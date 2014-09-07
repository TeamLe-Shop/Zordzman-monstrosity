#ifndef _SPRITE_HPP_
#define _SPRITE_HPP_

#include "project_config.hpp" 
#include "lin_alg.hpp"
#include "vertex.hpp"
#include <vector>

namespace zm {

	typedef Quad<float> Sprite, Plane;

	typedef std::vector<Sprite> SpriteSheet;

	class Surface {

		Vertex2F vertex[6];
	};



} /* namespace zm */ 

#endif /* #ifndef _SPRITE_HPP_ */