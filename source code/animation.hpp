#ifndef _ANIMATION_HPP_
#define _ANIMATION_HPP_

#include "project_config.hpp"
#include "sprite.hpp"

namespace zm {

	class Animation {

		public :

		/* load animation from file */
		bool load ( std::string const & filename ) {

			FILE * file;

			/* open file in binary mode */
			file = fopen(filename.c_str(),"rb");

			/* go to the end of the file */
			fseek(file,0,SEEK_END);

			/* get file size */
			long const size = ftell(file);

			/* go to the beginning of the file */
			fseek(file,0,SEEK_SET);

			uint32_t bytes_read = 0;

			/* call other function to load data */
			bool ld = (this)->load(file,size,bytes_read);

			/* close file */
			fclose(file);

			return ld;
		};

		/* load animation from data pointer, data_read returns the number of bytes consumed by this load */
		bool load ( FILE * file, uint32_t const size, uint32_t & bytes_read ) {

			bytes_read = 0;

			/* if size is too small to fit animation with one frame */
			if(size < 46)

				return false;

			/* the number of frames this animation has */
			uint32_t number_of_frames = 0;

			/* read animation data */
			fread(&((this)->run_time),sizeof(uint32_t),1,file);

			fread(&((this)->is_looping),sizeof(bool),1,file);

			fread(&((this)->is_zigzag),sizeof(bool),1,file);

			fread(&((this)->def_frame),sizeof(uint32_t),1,file);

			fread(&number_of_frames,sizeof(uint32_t),1,file);

			bytes_read = bytes_read + 14;

			/* if size is too small to fit number_of_frames */
			if(size - bytes_read < number_of_frames * sizeof(Sprite))

				return false;

			frames.resize(number_of_frames);

			/* read animation frames */
			fread(frames.data(),sizeof(Sprite),number_of_frames,file);

			return true;
		};



		/* keep track when animation started */
		uint32_t start_time;

		/* how long does it take to run this animation from first frame to the last */
		uint32_t run_time;

		/* if set run animation in loop */
		bool is_looping;

		/* if set run animation from beginning to end and beginning */
		bool is_zigzag;

		/* default animations frame id */ 
		uint32_t def_frame;

		/* animation frames */
		SpriteSheet frames;
	};

} /* namespace zm */ 

#endif /* #ifndef _ANIMATION_HPP_ */