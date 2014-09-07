#ifndef _GLSL_PROGRAM_HPP_
#define _GLSL_PROGRAM_HPP_

#include "glsl_shader.hpp"
#include "lin_alg.hpp"

namespace zm {

	class GLSL_program {

		public :

		enum {

			ER_NOT_SHADER = 1,
			ER_NOT_PROGRAM,
			ER_LINK,

		};

		GLSL_program () { };

		operator bool () const { return (this)->is_program(); };

		bool is_program () const { return (bool)glIsProgram((this)->program); };

		uint32_t create () {

			(this)->program = glCreateProgram();

			if(!(this)->is_program())

				return ER_NOT_PROGRAM;

			return 0;
		};

		void destroy () {

			if((this)->is_program()) {

				glDeleteProgram((this)->program);

				(this)->program = 0;
			}
		};

		uint32_t attach (GLSL_shader const shader) const {

			if(!shader)

				return ER_NOT_SHADER;

			if(!(this)->is_program())

				return ER_NOT_PROGRAM;

			glAttachShader((this)->program,shader.shader);

			return 0;
		};

		uint32_t detach (GLSL_shader const shader) const {

			if(!shader)

				return ER_NOT_SHADER;

			if(!(this)->is_program())

				return ER_NOT_PROGRAM;

			glDetachShader((this)->program,shader.shader);

			return 0;
		};

		uint32_t link () const {

			if(!(this)->is_program())

				return ER_NOT_PROGRAM;

			glLinkProgram((this)->program);

			int32_t status = GL_TRUE;

			glGetProgramiv((this)->program,GL_LINK_STATUS,&status);

			return status == GL_TRUE ? 0 : ER_LINK;
		};

		std::string info () const {

			if(!(this)->is_program())

				return "not a program";

			int32_t length = 0;

			glGetProgramiv((this)->program,GL_INFO_LOG_LENGTH,&length);

			char * log = new(std::nothrow) char[length];

			if(!log)

				return "storage allocation error";

			glGetProgramInfoLog((this)->program,length,nullptr,log);

			std::string const log_str = std::string(log,length);

			delete [] log;

			return log_str;
		};

		uint32_t use () const {

			if(!(this)->is_program())

				return ER_NOT_PROGRAM;

			glUseProgram((this)->program);

			return 0;
		};

		static uint32_t use (uint32_t const prog) {

			if(!glIsProgram(prog))

				return ER_NOT_PROGRAM;

			glUseProgram(prog);

			return 0;
		};

		int32_t get_uniform_location (std::string const & name) {

			if(!(this)->is_program())

				return 0;

			return glGetUniformLocation((this)->program,name.c_str());
		};

		template < typename _T > static void set_uniform (uint32_t const location, _T const & value) { static_assert(false, "Invalid _T type"); };

		template < > static void set_uniform < int32_t  > (uint32_t const location, int32_t  const & value) { glUniform1i(location,value); };
		template < > static void set_uniform < Vector2S > (uint32_t const location, Vector2S const & value) { glUniform2i(location,value.x,value.y); };
		template < > static void set_uniform < Vector3S > (uint32_t const location, Vector3S const & value) { glUniform3i(location,value.x,value.y,value.z); };
		template < > static void set_uniform < Vector4S > (uint32_t const location, Vector4S const & value) { glUniform4i(location,value.x,value.y,value.z,value.w); };

		template < > static void set_uniform < float	> (uint32_t const location, float	 const & value) { glUniform1f(location,value); };
		template < > static void set_uniform < Vector2F > (uint32_t const location, Vector2F const & value) { glUniform2f(location,value.x,value.y); };
		template < > static void set_uniform < Vector3F > (uint32_t const location, Vector3F const & value) { glUniform3f(location,value.x,value.y,value.z); };
		template < > static void set_uniform < Vector4F > (uint32_t const location, Vector4F const & value) { glUniform4f(location,value.x,value.y,value.z,value.w); };

		template < typename _T > static void set_uniform (uint32_t const location, _T const * const value, uint32_t const count) { static_assert(false, "Invalid _T type"); };

		template < > static void set_uniform < int32_t	> (uint32_t const location, int32_t  const * const value, uint32_t const count) { glUniform1iv(location,count,value); };
		template < > static void set_uniform < Vector2S > (uint32_t const location, Vector2S const * const value, uint32_t const count) { glUniform2iv(location,count,(int32_t*)value); };
		template < > static void set_uniform < Vector3S > (uint32_t const location, Vector3S const * const value, uint32_t const count) { glUniform3iv(location,count,(int32_t*)value); };
		template < > static void set_uniform < Vector4S > (uint32_t const location, Vector4S const * const value, uint32_t const count) { glUniform4iv(location,count,(int32_t*)value); };

		template < > static void set_uniform < float	> (uint32_t const location, float	 const * const value, uint32_t const count) { glUniform1fv(location,count,value); };
		template < > static void set_uniform < Vector2F > (uint32_t const location, Vector2F const * const value, uint32_t const count) { glUniform2fv(location,count,(float*)value); };
		template < > static void set_uniform < Vector3F > (uint32_t const location, Vector3F const * const value, uint32_t const count) { glUniform3fv(location,count,(float*)value); };
		template < > static void set_uniform < Vector4F > (uint32_t const location, Vector4F const * const value, uint32_t const count) { glUniform4fv(location,count,(float*)value); };

		template < > static void set_uniform < Matrix2x2S > (uint32_t const location, Matrix2x2S const * const value, uint32_t const count) { glUniform2iv(location,count,(int32_t*)value); };
		template < > static void set_uniform < Matrix3x3S > (uint32_t const location, Matrix3x3S const * const value, uint32_t const count) { glUniform3iv(location,count,(int32_t*)value); };
		template < > static void set_uniform < Matrix4x4S > (uint32_t const location, Matrix4x4S const * const value, uint32_t const count) { glUniform4iv(location,count,(int32_t*)value); };

		template < > static void set_uniform < Matrix2x2F > (uint32_t const location, Matrix2x2F const * const value, uint32_t const count) { glUniform2fv(location,count,(float*)value); };
		template < > static void set_uniform < Matrix3x3F > (uint32_t const location, Matrix3x3F const * const value, uint32_t const count) { glUniform3fv(location,count,(float*)value); };
		template < > static void set_uniform < Matrix4x4F > (uint32_t const location, Matrix4x4F const * const value, uint32_t const count) { glUniform4fv(location,count,(float*)value); };

		uint32_t program;
	};

} /* namespace zm */ 

#endif