#ifndef _VERTEX_HPP_
#define _VERTEX_HPP_

#include "project_config.hpp"
#include "lin_alg.hpp"

namespace zm {

	template < uint32_t pdim, uint32_t cdim, uint32_t tdim, typename type > struct Vertex {

		Vector<pdim,type> postion;
		Vector<cdim,int8_t> color;
		Vector<tdim,float> tcoord;
	};

	typedef Vertex<2,4,2,int32_t>	Vertex2S;
	typedef Vertex<2,4,2,uint32_t>	Vertex2U;
	typedef Vertex<2,4,2,float>		Vertex2F;
	typedef Vertex<2,4,2,double>	Vertex2D;

	typedef Vertex<3,4,2,int32_t>	Vertex3S;
	typedef Vertex<3,4,2,uint32_t>	Vertex3U;
	typedef Vertex<3,4,2,float>		Vertex3F;
	typedef Vertex<3,4,2,double>	Vertex3D;

} /* namespace zm */

#endif /* #ifndef _VERTEX_HPP_ */