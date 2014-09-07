#ifndef _GLSL_SHADER_HPP_
#define _GLSL_SHADER_HPP_

#include <string>
#include <sstream>
#include <fstream>
#include <cstdint>

#include "project_config.hpp"

namespace zm {

	class GLSL_shader {

		public :

		enum {

			ST_FRAGMENT = GL_FRAGMENT_SHADER,
			ST_VERTEX = GL_VERTEX_SHADER,
		};

		enum {

			ER_NOT_SHADER = 1,
			ER_OPEN_FILE,
			ER_EMPTY_FILE,
			ER_ALLOC,
			ER_COMPILE
		};

		GLSL_shader() { };

		GLSL_shader(uint32_t const type, std::string const & filename) {
	
			(this)->create(type,filename);
		};

		operator bool() const { return (this)->is_shader(); };

		uint32_t create (uint32_t const type) {

			(this)->shader = glCreateShader(type);

			return glIsShader((this)->shader) ? 0 : ER_NOT_SHADER;
		};

		void destroy () {

			if((this)->is_shader()) {

				glDeleteShader((this)->shader);

				(this)->shader = 0;
			}
		};

		uint32_t load (std::string const & filename) const {

			if(!(this)->is_shader())

				return ER_NOT_SHADER;

			std::ifstream file(filename);

			if(!file)

				return ER_OPEN_FILE;

			file.seekg(file.end);

			uint32_t const size = (uint32_t)file.tellg();

			file.seekg(file.beg);

			if(size == 0)

				return ER_EMPTY_FILE;

			char * source = new(std::nothrow) char[size + 1];

			if(!source)

				return ER_ALLOC;

			file.read(source,size);

			source[size] = '\0';

			glShaderSource((this)->shader,1,const_cast<char const **>(&source),nullptr);

			delete [] source;

			return 0;
		};

		uint32_t compile () const {

			if(!(this)->is_shader())

				return ER_NOT_SHADER;

			glCompileShader((this)->shader);

			int32_t status = GL_TRUE;

			glGetShaderiv((this)->shader,GL_COMPILE_STATUS,&status);

			return status == GL_TRUE ? 0 : ER_COMPILE;
		};

		std::string info ( ) const {

			if(!(this)->is_shader())

				return "not a shader";

			int32_t length = 0;

			glGetShaderiv((this)->shader,GL_INFO_LOG_LENGTH,&length);

			char * log = new(std::nothrow) char[length];

			if(!log)

				return "storage allocation error";

			glGetShaderInfoLog((this)->shader,length,nullptr,log);

			std::string const log_str = std::string(log,length);

			delete [] log;

			return log_str;
		};

		uint32_t create (std::string const & filename) const {

			uint32_t const lerr = (this)->load(filename);

			if(lerr != 0) 

				return lerr;

			uint32_t const cerr = (this)->compile();

			if(cerr != 0) 

				return cerr;

			return 0;
		};

		uint32_t create (uint32_t const type, std::string const & filename) {

			uint32_t const cerr = (this)->create(type);

			if(cerr != 0) 

				return cerr;

			uint32_t const perr = (this)->create(filename);

			if(perr != 0) 

				return perr;

			return 0;
		};

		bool is_shader () const {

			return glIsShader((this)->shader);
		}

		uint32_t shader;
	};

} /* namespace zm */

#endif