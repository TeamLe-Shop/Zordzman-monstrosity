#ifndef _ANIMATION_HPP_
#define _ANIMATION_HPP_

#include "project_config.hpp"
#include "sprite.hpp"
#include <fstream>

namespace zm {

	class Animation {

		public :

		/* load animation from file */
		bool load ( std::string const & filename ) {

			FILE * file;

			/* open file in binary mode */
			file = fopen(filename.c_str(),"rb");

			if(!file)

				return false;

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

		/* load animation from file pointer, bytes_read returns the number of bytes consumed by this load */
		bool load ( FILE * file, uint32_t const size, uint32_t & bytes_read ) {

			bytes_read = 0;

			/* if size is too small to fit animation with one frame */
			if(size < 45)

				return false;

			/* the number of frames this animation has */
			uint32_t number_of_frames = 0;

			/* read animation data */
			fread(&((this)->run_time),sizeof(uint32_t),1,file);

			fread(&((this)->is_looping),sizeof(bool),1,file);

			fread(&((this)->def_frame),sizeof(uint32_t),1,file);

			fread(&number_of_frames,sizeof(uint32_t),1,file);

			bytes_read = bytes_read + 13;

			/* if size is too small to fit number_of_frames */
			if(size - bytes_read < number_of_frames * sizeof(Sprite))

				return false;

			frames.resize(number_of_frames);

			/* read animation frames */
			fread(frames.data(),sizeof(Sprite),number_of_frames,file);

			bytes_read = bytes_read + number_of_frames * sizeof(Sprite);

			return true;
		};

		/* store animation in file */
		bool store ( std::string const & filename ) {

			FILE * file;

			/* open file in binary mode */
			file = fopen(filename.c_str(),"wb");

			if(!file)

				return false;

			uint32_t bytes_written = 0;

			/* call other function to load data */
			bool st = (this)->store(file,UINT32_MAX,bytes_written);

			/* close file */
			fclose(file);

			return st;
		};

		/* write animation in to file pointer, bytes_written returns the number of bytes written by this store */
		bool store ( FILE * file, uint32_t const size, uint32_t & bytes_written ) const {

			bytes_written = 0;

			/* if size is too small to fit animation with one frame */
			if(size < 45)

				return false;

			/* the number of frames this animation has */
			uint32_t number_of_frames = (this)->frames.size();

			/* write animation data */
			fwrite(&((this)->run_time),sizeof(uint32_t),1,file);

			fwrite(&((this)->is_looping),sizeof(bool),1,file);

			fwrite(&((this)->def_frame),sizeof(uint32_t),1,file);

			fwrite(&number_of_frames,sizeof(uint32_t),1,file);

			bytes_written = bytes_written + 13;

			/* if size is too small to fit number_of_frames */
			if(size - bytes_written < number_of_frames * sizeof(Sprite))

				return false;

			/* write animation frames */
			fwrite(frames.data(),sizeof(Sprite),number_of_frames,file);

			bytes_written = bytes_written + number_of_frames * sizeof(Sprite);

			return true;
		};

		/* store animation file in readable format for debugging purposes */
		bool store_debug ( std::string const & filename ) {

			std::ofstream file(filename);

			file << (this)->run_time << ' ' << (this)->is_looping << ' ' << (this)->def_frame << ' ' << (this)->frames.size() << std::endl << std::endl << std::fixed;

			for(unsigned int i = 0; i < (this)->frames.size(); i++) {

				file << (this)->frames[i].points[0].x << ' ' << (this)->frames[i].points[0].y << ' ' << (this)->frames[i].points[1].x << ' ' << (this)->frames[i].points[1].y << std::endl;
				file << (this)->frames[i].points[3].x << ' ' << (this)->frames[i].points[3].y << ' ' << (this)->frames[i].points[2].x << ' ' << (this)->frames[i].points[2].y << std::endl << std::endl;
			}

			file.close();

			return true;
		}

		/* load debug animation file */
		bool load_debug ( std::string const & filename ) {

			std::ifstream file(filename);

			uint32_t size = 0;

			file >> (this)->run_time >> (this)->is_looping >> (this)->def_frame >> size;

			(this)->frames.resize(size);

			for(unsigned int i = 0; i < size; i++) {

				file >> (this)->frames[i].points[0].x >> (this)->frames[i].points[0].y >> (this)->frames[i].points[1].x >> (this)->frames[i].points[1].y;
				file >> (this)->frames[i].points[3].x >> (this)->frames[i].points[3].y >> (this)->frames[i].points[2].x >> (this)->frames[i].points[2].y;
			}

			file.close();

			return true;
		}

		/* automatically generate frames, function will not fail even if (sprite_count.x * sprite_count.y) < frame_count */
		void generate_frames ( Vector2U const & texture_wh, Vector2U const & tex_offset, Vector2U const & sprite_wh, Vector2U const & sprite_spaces_wh, Vector2U const & sprite_count, uint32_t const & frame_count, bool const & horizontal = true ) {

			/* translate pixel based coordinates in to uv space coordinates */
			Vector2F const texture_pixel_uv = Vector2F(1.0f) / Vector2F(texture_wh);

			Vector2F const tex_offset_uv = texture_pixel_uv * Vector2F(tex_offset);

			Vector2F const sprite_wh_uv = texture_pixel_uv * Vector2F(sprite_wh);

			Vector2F const sprite_spaces_wh_uv = texture_pixel_uv * Vector2F(sprite_spaces_wh);

			Vector2F sprite_offset = tex_offset_uv;

			(this)->frames.reserve(frame_count);

			/* read frames horizontally */
			if(horizontal) {

				for(unsigned int i = 0; i < frame_count; i++) {

					Sprite sprite(sprite_offset,sprite_offset + sprite_wh_uv);

					(this)->frames.push_back(sprite);

					/* if this is the last frame in a row */
					if((i % sprite_count.x) == (sprite_count.x - 1)) {

						/* get back to front, incremet y by one row */
						sprite_offset = Vector2F(tex_offset_uv.x,sprite_offset.y + sprite_wh_uv.y + sprite_spaces_wh_uv.y);
					}

					/* if this frame is not the last one */
					else {

						sprite_offset.translate_x(sprite_wh_uv.x + sprite_spaces_wh_uv.x);
					}
				}
			}

			/* read frames vertically */
			else {

				for(unsigned int i = 0; i < frame_count; i++) {

					Sprite sprite(sprite_offset,sprite_offset + sprite_wh_uv);

					(this)->frames.push_back(sprite);

					/* if this is the last frame in a collumn */
					if((i % sprite_count.y) == (sprite_count.y - 1)) {

						/* get back to front, incremet x by one collumn */
						sprite_offset = Vector2F(sprite_offset.x + sprite_wh_uv.x + sprite_spaces_wh_uv.x,tex_offset_uv.y);
					}

					/* if this frame is not the last one */
					else {

						sprite_offset.translate_y(sprite_wh_uv.y + sprite_spaces_wh_uv.y);
					}
				}
			}
		}

		/* set frames[frame_id] as current frame */
		void set_current_frame ( uint32_t const & frame_id ) {

			if((this)->is_looping) {

				/* bring frame_id to range 0 and frames.size() */
				uint32_t const looping_frame_id = frame_id % frames.size();

				/* calculate how long it takes to show one frame */
				uint32_t const frame_time = (this)->run_time / frames.size();

				/* calculate how long it takes to show animation to looping_frame_id as current frame */
				uint32_t const animation_run_time = looping_frame_id * frame_time;

				/* set current frame */
				(this)->start_time = (_GET_UTC_TIME_MS_ - animation_run_time);
			}

			else {

				/* if frame_id is out of range 0 and frames.size() */
				if(frame_id > frames.size())

					/* frame_id is out of range, stop animation */
					(this)->stop();

				/* calculate how long it takes to show one frame */
				uint32_t const frame_time = (this)->run_time / frames.size();

				/* calculate how long it takes to show animation to frame_id as current frame */
				uint32_t const animation_run_time = frame_id * frame_time;

				/* set current frame */
				(this)->start_time = (_GET_UTC_TIME_MS_ - animation_run_time);
			}
		};

		/* returns true if animation current frame is default frame, otherwise false */
		void show_default_frame ( ) {

			/* if this animation is not looping */
			if(!(this)->is_looping) {

				(this)->stop();
			}
		}

		/* returns true if animation current frame is default frame, otherwise false */
		bool is_default_frame ( ) {

			if((this)->is_looping)

				return false;

			/* check if animation had been running longer then run_time */
			return (_GET_UTC_TIME_MS_ - (this)->start_time > (this)->run_time);
		}

		Sprite get_frame ( ) {

			/* calculate how long it takes to show one frame */
			uint32_t const frame_time = (this)->run_time / frames.size();

			/* calculate how long animation had been running */
			uint32_t const animation_run_time = _GET_UTC_TIME_MS_ - (this)->start_time;

			if((this)->is_looping)

				return (this)->frames[((animation_run_time / frame_time) % (this)->frames.size())];

			if(animation_run_time > (this)->run_time)

				return (this)->frames[(this)->def_frame];

			return (this)->frames[animation_run_time / frame_time];
		}

		/* start animation */
		void start ( ) {

			(this)->start_time = _GET_UTC_TIME_MS_;
		};

		/* stop animation */
		void stop ( ) {

			/* assuming we are dealing with utc time, not timestamps */
			(this)->start_time = 0;
		};

		void clean ( ) { (this)->frames.clear(); };

		Animation & operator ++ ( ) {

			/* calculate how long it take to show one frame */
			uint32_t const frme_time = (this)->run_time / frames.size();

			/* increment by one frame */
			(this)->start_time = (this)->start_time - frme_time;

			return (*this);
		};

		Animation operator + ( uint32_t const & frame_count ) const {

			/* make a coppy of this */
			Animation cpy = (*this);

			/* calculate how long it take to show one frame */
			uint32_t const frme_time = (this)->run_time / frames.size();

			/* increment by one frame */
			cpy.start_time = (this)->start_time - (frme_time * frame_count);

			return cpy;
		};

		Animation operator - ( uint32_t const & frame_count ) const {

			/* make a coppy of this */
			Animation cpy = (*this);

			/* calculate how long it take to show one frame */
			uint32_t const frme_time = (this)->run_time / frames.size();

			/* increment by one frame */
			cpy.start_time = (this)->start_time + (frme_time * frame_count);

			return cpy;
		};

		Animation & operator += ( uint32_t const & frame_count ) {

			/* calculate how long it take to show one frame */
			uint32_t const frme_time = (this)->run_time / frames.size();

			/* increment by one frame */
			(this)->start_time = (this)->start_time - (frme_time * frame_count);

			return (*this);
		};

		Animation & operator -= ( uint32_t const & frame_count ) {

			/* calculate how long it take to show one frame */
			uint32_t const frme_time = (this)->run_time / frames.size();

			/* increment by one frame */
			(this)->start_time = (this)->start_time + (frme_time * frame_count);

			return (*this);
		};

		/* keep track when animation started */
		uint32_t start_time;

		/* how long it takes to run this animation from first frame to the last */
		uint32_t run_time;

		/* if set run animation in loop */
		bool is_looping;

		/* default animations frame id */ 
		uint32_t def_frame;

		/* animation frames */
		SpriteSheet frames;
	};

	typedef std::vector<Animation> AnimationSheet;

} /* namespace zm */ 

#endif /* #ifndef _ANIMATION_HPP_ */