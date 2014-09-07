#ifndef _ANIMATION_HPP_
#define _ANIMATION_HPP_

#include "project_config.hpp"
#include "sprite.hpp"
#include <fstream>
#include "application.hpp"

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
			uint32_t frame_time = (this)->run_time / frames.size();

			if(frame_time == 0)

				frame_time = 1;

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

		bool edit ( Window * parent );

		static bool editing;
	};

	bool Animation::editing = false;

	typedef std::vector<Animation> AnimationSheet;

	void post_rendering_message ( Window * window ) {

		/* kick start the rendering process */
		Event event;

		event.type = ZM_WINDOW_EVENT;

		event.zm_event.window.event = ZM_WINDOW_EVENT_RENDER;

		event.zm_event.window.window_id = window->id;

		SDL_PushEvent(&event.sdl_event);
	}

}

#include <iostream>

namespace zm {

	namespace animation_editor {

		int const window_width = 1440;

		int const window_height = 800;

		int const texture_window_width = 300;

		int const texture_window_height = 600;

		Vector2F const uv_window_pos(float(texture_window_width),0);

		int const uv_window_width = window_width - texture_window_width;

		int const uv_window_height = texture_window_height;

		Vector2F const frame_window_pos(0,float(uv_window_height + 2));

		int const frame_window_width = 1200;

		int const frame_window_height = window_height - texture_window_height;


		int const preview_window_width = window_width - (frame_window_width + 1);

		int const preview_window_height = window_height - (texture_window_height + 1);

		Vector2F const preview_window_pos(float(frame_window_width + 1),float(frame_window_pos.y));



		Animation * current_object;

		enum EditorAction {

			NO_ACTION,

			FOCUS,

			TEXTURE_LIST_FOCUS = 3,

			TEXTURE_LIST_SCROLLING, /* scrolling by moving scrollbar */

			UV_EDITOR_FOCUS,

			UV_EDITOR_DRAG,

			UV_EDITOR_SPACING1,

			UV_EDITOR_DRAG_POINT,

			UV_EDITOR_SPACING2,
			
			UV_EDITOR_MASK_DRAG,

			FRAME_LIST_FOCUS,

			FRAME_LIST_SCROLLING,

			PREVIEW_WINDOW_FOCUS,
		};

		int32_t current_action = EditorAction::NO_ACTION;

		uint32_t load_texture ( std::string const & filename, Vector2F & wh ) {
		
			SDL_Surface * surface = IMG_Load(filename.c_str());

			if(surface != nullptr) {

				wh = Vector2F((float)surface->w,(float)surface->h);

				uint32_t tex = create_texture(surface);

				SDL_FreeSurface(surface);

				return tex;
			}

			return 0;
		}

		struct Texture {

			uint32_t texture;

			Vector2F wh;
		};

		class TextureList {

			public :

			TextureList ( ) {
			
				init();
			}

			void add ( Texture const & texture ) {

				textures.push_back(texture);

				float last_size = size;

				size = size + texture.wh.normalized().y * 256 + texture_plane_spacing.y;

				float ratio = last_size / size;

				scrollbar_possition.y = scrollbar_possition.y * ratio;

			}

			void init ( ) {
			
				current_texture = 0;

				current_id = -1;

				plane = Plane(Vector2F(0,0),Vector2F((float)texture_window_width,(float)texture_window_height));

				lsit_draw_offset_y = 40;

				texture_plane = Plane(Vector2F(0,0),Vector2F(128,128));

				texture_plane_spacing = Vector2F(0,40);

				size = 1;

				current_action = EditorAction::NO_ACTION;

				scrollbar_possition = Vector2F(texture_window_width - 17,0);
			}

			void draw ( ) {

				glLineWidth(2.0f);

				/* disable texturing */
				bind_texture(0);

				glBegin(GL_LINES);

					glColor3f(0.6f,0.6f,0.6f);

					glVertex2f(float(texture_window_width),0);
					glVertex2f(float(texture_window_width),float(texture_window_height + 1));

					glVertex2f(0,float(texture_window_height + 1));
					glVertex2f(float(texture_window_width),float(texture_window_height + 1));

				glEnd();

				glEnable(GL_SCISSOR_TEST);

				/* do not draw outside this rectangle */
				glScissor(0,window_height - texture_window_height,texture_window_width,window_height);

				/* draw scrollbar */

				/* find out the scrollbar scale compared to texture_window_height */
				float scale = texture_window_height / size; // scale should be a number between one and zero

				if(scale < 1) {

					/* find out the size of the scrollbar */
					float scrollbar_size = texture_window_height * (texture_window_height / (size + texture_plane_spacing.y));

					Plane scrollbar_plane = Plane(Vector2F(0,0),Vector2F(15,scrollbar_size)) + scrollbar_possition;

					glBegin(GL_QUADS);

						glColor3f(0.5f,0.5f,0.5f);

						glVertex2f(scrollbar_plane.points[0].x,scrollbar_plane.points[0].y);
						glVertex2f(scrollbar_plane.points[1].x,scrollbar_plane.points[1].y);
						glVertex2f(scrollbar_plane.points[2].x,scrollbar_plane.points[2].y);
						glVertex2f(scrollbar_plane.points[3].x,scrollbar_plane.points[3].y);

					glEnd();

				}
				float translation_y = lsit_draw_offset_y;

				glColor3f(1.0f,1.0f,1.0f);

				for(unsigned int i = 0; i < textures.size(); i++) {

					Vector2F wh = textures[i].wh.normalized() * 256;

					float pos_x = ((plane.points[2].x - plane.points[0].x - wh.x) / 2) - 15;

					Plane current_plane = Plane(Vector2F(0,0),wh) + Vector2F(pos_x,translation_y);

					if(current_id == i) {

						glLineWidth(2.0f);

						bind_texture(0);

						Plane highlight_plane = Plane(current_plane.points[0] - Vector2F(1,1),current_plane.points[2] + Vector2F(1,1));

						/* draw texture quads */
						glBegin(GL_LINE_LOOP);

						glColor3f(0.0f,0.66f,1.0f);

						glVertex2f(highlight_plane.points[0].x,highlight_plane.points[0].y);
						glVertex2f(highlight_plane.points[1].x,highlight_plane.points[1].y);
						glVertex2f(highlight_plane.points[2].x,highlight_plane.points[2].y);
						glVertex2f(highlight_plane.points[3].x,highlight_plane.points[3].y);

						glEnd();

						glColor3f(1,1,1);
					}

					/* draw white rectangle underneath the textures */
					glBegin(GL_QUADS);

					glColor3f(1.0f,1.0f,1.0f);

					glVertex2f(current_plane.points[0].x,current_plane.points[0].y);
					glVertex2f(current_plane.points[1].x,current_plane.points[1].y);
					glVertex2f(current_plane.points[2].x,current_plane.points[2].y);
					glVertex2f(current_plane.points[3].x,current_plane.points[3].y);

					glEnd();

					bind_texture(textures[i].texture);

					/* draw texture quads */
					glBegin(GL_QUADS);

					glColor3f(1.0f,1.0f,1.0f);

					glTexCoord2f(0,0);
					glVertex2f(current_plane.points[0].x,current_plane.points[0].y);

					glTexCoord2f(1,0);
					glVertex2f(current_plane.points[1].x,current_plane.points[1].y);

					glTexCoord2f(1,1);
					glVertex2f(current_plane.points[2].x,current_plane.points[2].y);

					glTexCoord2f(0,1);
					glVertex2f(current_plane.points[3].x,current_plane.points[3].y);

					glEnd();

					translation_y = translation_y + wh.y + texture_plane_spacing.y;

					bind_texture(0);

					glLineWidth(1);

					glBegin(GL_LINE_LOOP);

						glColor3f(0.5f,0.5f,0.5f);

						glVertex2f(current_plane.points[0].x - 1,current_plane.points[0].y);
						glVertex2f(current_plane.points[1].x + 1,current_plane.points[1].y);
						glVertex2f(current_plane.points[2].x + 1,current_plane.points[2].y);
						glVertex2f(current_plane.points[3].x - 1,current_plane.points[3].y);

					glEnd();
				}

				bind_texture(0);



				glDisable(GL_SCISSOR_TEST);
			}

			bool select_texture ( Vector2F const & pos ) {

				Vector2F const real_pos = pos;//.translated_y(-lsit_draw_offset_y);

				float translation_y = lsit_draw_offset_y;

				for(unsigned int i = 0; i < textures.size(); i++) {

					Vector2F wh = textures[i].wh.normalized() * 256;

					float pos_x = (plane.points[2].x - plane.points[0].x - wh.x) / 2;

					Plane current_plane = Plane(Vector2F(0,0),wh) + Vector2F(pos_x,translation_y);

					if(current_plane.aligned_collides_point(real_pos)) {

						current_texture = textures[i].texture;

						current_id = i;

						return true;
					}

					translation_y = translation_y + wh.y + texture_plane_spacing.y;
				}

				return false;
			}

			bool is_on_scrollbar ( Vector2F const & mouse ) {

				if(texture_window_height < size) {

					float scrollbar_size = texture_window_height * (texture_window_height / (size + texture_plane_spacing.y));

					Plane scrollbar_plane = Plane(Vector2F(0,0),Vector2F(15,scrollbar_size)) + scrollbar_possition;

					return scrollbar_plane.aligned_collides_point(mouse);
				}

				return false;
			}

			void scroll_scrollbar ( int32_t const & mouse_rel_y ) {

				float scale = size / texture_window_height;

				/* find out the size of the scrollbar */
				float scrollbar_size = texture_window_height * (texture_window_height / (size + texture_plane_spacing.y));

				float prew_pos = scrollbar_possition.y;

				/* adjust scrollbar */
				scrollbar_possition.y = scrollbar_possition.y + mouse_rel_y;

				if(scrollbar_possition.y + scrollbar_size > texture_window_height)

					scrollbar_possition.y = texture_window_height - scrollbar_size;

				if(scrollbar_possition.y < 0)

					scrollbar_possition.y = 0;

				float diff = scrollbar_possition.y - prew_pos;

				lsit_draw_offset_y = lsit_draw_offset_y + (-diff * scale);
			}

			void scroll ( int32_t const & s ) {

				/* find out the size of the scrollbar */
				float scrollbar_size = texture_window_height * (texture_window_height / (size + texture_plane_spacing.y));

				/* find out the scrollbar scale compared to texture_window_height */
				float scale = texture_window_height / size; // scale should be a number between one and zero

				/* adjust scrollbar */
				scrollbar_possition.y = scrollbar_possition.y + ((-s * 64) * scale);

				/* limit scrollbar */
				if(scrollbar_possition.y + scrollbar_size > texture_window_height)

					scrollbar_possition.y = texture_window_height - scrollbar_size;

				if(scrollbar_possition.y < 0)

					scrollbar_possition.y = 0;

				/* perform scrolling */
				lsit_draw_offset_y = lsit_draw_offset_y + (s * 64);

				/* limit scrolling */
				if(lsit_draw_offset_y + size < texture_window_height)

					lsit_draw_offset_y = texture_window_height - size;

				if(lsit_draw_offset_y > texture_plane_spacing.y)

					lsit_draw_offset_y = texture_plane_spacing.y;
			}

			void remove_selected ( ) {
			
				if(current_id != -1) {

					glDeleteTextures(1,&textures[current_id].texture);

					float old_size = size;

					size = size - textures[current_id].wh.normalized().y * 256 - texture_plane_spacing.y;

					if(size < 1)

						size = 1;

					float ratio = old_size / size;

					scrollbar_possition.y = scrollbar_possition.y / ratio;

					textures.erase(textures.begin() + current_id);

					current_id = -1;

					current_texture = 0;

					scroll(0);
				}
			}

			uint32_t last_mouse_y;

			uint32_t current_texture;

			int32_t current_id;

			Plane plane;

			float size;

			float lsit_draw_offset_y;

			Plane texture_plane;

			Vector2F texture_plane_spacing;

			std::vector<Texture> textures;

			Vector2F scrollbar_possition;

			Plane scrollbar_plane;
		};

		TextureList texture_list;

		class FrameList {

			public :

			/* requires explicit initialization */
			void init ( ) {

				current_frame_id = -1;

				sprite_sheet = nullptr;

				draw_offset = frame_window_pos.translated_y(50);

				plane = Plane(frame_window_pos,Vector2F(float(frame_window_width),float(frame_window_height)) + frame_window_pos);

				frame_plane_spacing = Vector2F(40,0);

				sprite_sheet = &current_object->frames;

				scrollbar_possition = frame_window_pos.translated_y(1);

				size = (current_object->frames.size() + 1) * (128 + frame_plane_spacing.x) + frame_plane_spacing.x;
			}

			void draw ( ) {

				glLineWidth(2);

				/* draw last line */

				glBegin(GL_LINES);

					glColor3f(0.6f,0.6f,0.6f);

					glVertex2f(frame_window_pos.x + frame_window_width,frame_window_pos.y);
					glVertex2f(frame_window_pos.x + frame_window_width,frame_window_pos.y + frame_window_height);

				glEnd();

				glEnable(GL_SCISSOR_TEST);

				/* do not draw outside this rectangle */
				glScissor(0,0,frame_window_width,frame_window_height);

				bind_texture(0);

				if(frame_window_width < size) {

					/* find out the size of the scrollbar */
					float scrollbar_size = frame_window_width * (frame_window_width / (size + frame_plane_spacing.y));

					Plane scrollbar_plane = Plane(Vector2F(0,0),Vector2F(scrollbar_size,15)) + scrollbar_possition;

					/* draw scrollbar */

					glBegin(GL_QUADS);

						glColor3f(0.5f,0.5f,0.5f);

						glVertex2f(scrollbar_plane.points[0].x,scrollbar_plane.points[0].y);
						glVertex2f(scrollbar_plane.points[1].x,scrollbar_plane.points[1].y);
						glVertex2f(scrollbar_plane.points[2].x,scrollbar_plane.points[2].y);
						glVertex2f(scrollbar_plane.points[3].x,scrollbar_plane.points[3].y);

					glEnd();

				}

				float cur_loccation_x = draw_offset.x + frame_plane_spacing.x;

				glLineWidth(1);



				/* drw list */
				if(texture_list.current_id != -1) {

					cur_loccation_x = draw_offset.x + frame_plane_spacing.x;

					/* draw frames */

					for(unsigned int i = 0; i < sprite_sheet->size(); i++) {

						Plane temp_frame_plane = Plane(Vector2F(0,0),Vector2F(128,128)) + (Vector2F(cur_loccation_x,draw_offset.y));

						/* draw frame backgrounds */

						glBegin(GL_QUADS);

							glColor3f(1.0,1.0,1.0);

							glVertex2f(temp_frame_plane.points[0].x,temp_frame_plane.points[0].y);
							glVertex2f(temp_frame_plane.points[1].x,temp_frame_plane.points[1].y);
							glVertex2f(temp_frame_plane.points[2].x,temp_frame_plane.points[2].y);
							glVertex2f(temp_frame_plane.points[3].x,temp_frame_plane.points[3].y);

						glEnd();

						bind_texture(texture_list.current_texture);

						glBegin(GL_QUADS);

							glColor3f(1.0,1.0,1.0);

							glTexCoord2f((*sprite_sheet)[i].points[0].x,(*sprite_sheet)[i].points[0].y);
							glVertex2f(temp_frame_plane.points[0].x,temp_frame_plane.points[0].y);

							glTexCoord2f((*sprite_sheet)[i].points[1].x,(*sprite_sheet)[i].points[1].y);
							glVertex2f(temp_frame_plane.points[1].x,temp_frame_plane.points[1].y);

							glTexCoord2f((*sprite_sheet)[i].points[2].x,(*sprite_sheet)[i].points[2].y);
							glVertex2f(temp_frame_plane.points[2].x,temp_frame_plane.points[2].y);

							glTexCoord2f((*sprite_sheet)[i].points[3].x,(*sprite_sheet)[i].points[3].y);
							glVertex2f(temp_frame_plane.points[3].x,temp_frame_plane.points[3].y);

						glEnd();

						bind_texture(0);

						/* draw frame bounds */

						glBegin(GL_LINE_LOOP);

							glColor3f(0.5f,0.5f,0.5f);

							glVertex2f(temp_frame_plane.points[0].x,	temp_frame_plane.points[0].y);
							glVertex2f(temp_frame_plane.points[1].x + 1,temp_frame_plane.points[1].y);
							glVertex2f(temp_frame_plane.points[2].x + 1,temp_frame_plane.points[2].y + 1);
							glVertex2f(temp_frame_plane.points[3].x,	temp_frame_plane.points[3].y + 1);

						glEnd();

						if(current_frame_id == i) {

							glLineWidth(2.0f);

							Plane highlight_plane = Plane(temp_frame_plane.points[0] - Vector2F(1,1),temp_frame_plane.points[2] + Vector2F(1,1));

							/* draw texture quads */
							glBegin(GL_LINE_LOOP);

							glColor3f(0.0f,0.66f,1.0f);

							glVertex2f(highlight_plane.points[0].x,highlight_plane.points[0].y);
							glVertex2f(highlight_plane.points[1].x,highlight_plane.points[1].y);
							glVertex2f(highlight_plane.points[2].x,highlight_plane.points[2].y);
							glVertex2f(highlight_plane.points[3].x,highlight_plane.points[3].y);

							glEnd();

							glLineWidth(1);
						}

						cur_loccation_x = cur_loccation_x + 128 + frame_plane_spacing.x;
					}

					bind_texture(0);
				}

				/* draw add new frame rect */

				Plane temp_frame_plane = Plane(Vector2F(0,0),Vector2F(128,128)) + (Vector2F(cur_loccation_x,draw_offset.y));

				glBegin(GL_QUADS);

					glColor3f(0.0f,0.66f,1.0f);

					glVertex2f(temp_frame_plane.points[0].x,temp_frame_plane.points[0].y);
					glVertex2f(temp_frame_plane.points[1].x,temp_frame_plane.points[1].y);
					glVertex2f(temp_frame_plane.points[2].x,temp_frame_plane.points[2].y);
					glVertex2f(temp_frame_plane.points[3].x,temp_frame_plane.points[3].y);

				glEnd();

				glLineWidth(10);

				glBegin(GL_LINES);

					Vector2F cur_loc = Vector2F(cur_loccation_x,draw_offset.y);

					glColor3f(1.0f,1.0f,1.0f);

					glVertex2f(cur_loc.x + 64,temp_frame_plane.points[0].y + 20);
					glVertex2f(cur_loc.x + 64,temp_frame_plane.points[2].y - 20);

					glVertex2f(cur_loc.x + 20, temp_frame_plane.points[0].y + 64);
					glVertex2f(cur_loc.x + 108,temp_frame_plane.points[0].y + 64);


				glEnd();

				glDisable(GL_SCISSOR_TEST);
			}

			void scroll_scrollbar ( int32_t const & mouse_rel_x ) {

				float scale = size / frame_window_width;

				/* find out the size of the scrollbar */
				float scrollbar_size = frame_window_width * (frame_window_width / (size + frame_plane_spacing.x));

				float prew_pos = scrollbar_possition.x;

				/* adjust scrollbar */
				scrollbar_possition.x = scrollbar_possition.x + mouse_rel_x;

				if(scrollbar_possition.x + scrollbar_size > frame_window_width)

					scrollbar_possition.x = frame_window_width - scrollbar_size;

				if(scrollbar_possition.x < 0)

					scrollbar_possition.x = 0;

				float diff = scrollbar_possition.x - prew_pos;

				draw_offset.x = draw_offset.x + (-diff * scale);
			}

			void scroll ( int32_t const & s ) {

				/* find out the size of the scrollbar */
				float scrollbar_size = frame_window_width * (frame_window_width / (size + frame_plane_spacing.x));

				float frame_window_width_fp = frame_window_width;
				
				/* find out the scrollbar scale compared to texture_window_height */
				float scale = frame_window_width / size; // scale should be a number between one and zero

				/* adjust scrollbar */
				scrollbar_possition.x = scrollbar_possition.x + ((-s * 64) * scale);

				/* limit scrollbar */
				if(scrollbar_possition.x + scrollbar_size > frame_window_width)

					scrollbar_possition.x = frame_window_width - scrollbar_size;

				if(scrollbar_possition.x < 0)

					scrollbar_possition.x = 0;

				/* perform scrolling */
				draw_offset.x = draw_offset.x + (s * 64);

				/* limit scrolling */
				if(draw_offset.x + size < frame_window_width)

					draw_offset.x = frame_window_width - size;

				if(draw_offset.x > 0)

					draw_offset.x = 0;
			}

			bool is_on_scrollbar ( Vector2F const & mouse ) {

				if(frame_window_width < size) {

					float scrollbar_size = frame_window_width * (frame_window_width / (size + frame_plane_spacing.x));

					Plane scrollbar_plane = Plane(Vector2F(0,0),Vector2F(scrollbar_size,15)) + scrollbar_possition;

					return scrollbar_plane.aligned_collides_point(mouse);
				}

				return false;
			}

			bool select_frame ( Vector2F const & mouse_xy ) {

				float cur_loccation_x = draw_offset.x + frame_plane_spacing.x;

				for(unsigned int i = 0; i < sprite_sheet->size(); i++) {

					Plane temp_frame_plane = Plane(Vector2F(0,0),Vector2F(128,128)) + (Vector2F(cur_loccation_x,draw_offset.y));

					if(temp_frame_plane.aligned_collides_point(mouse_xy)) {

						current_frame_id = i;

						return true;
					}

					cur_loccation_x = cur_loccation_x + 128 + frame_plane_spacing.x;
				}

				Plane temp_frame_plane = Plane(Vector2F(0,0),Vector2F(128,128)) + (Vector2F(cur_loccation_x,draw_offset.y));

				if(temp_frame_plane.aligned_collides_point(mouse_xy)) {

					add_frame(Plane(Vector2F(0,0),Vector2F(1,1)));

					return true;
				}

				return false;
			}

			void add_frame ( Plane const & frame, std::vector<Plane>::iterator const & pos = current_object->frames.end()) {

				sprite_sheet->insert(pos,frame);

				float last_size = size;

				size = (current_object->frames.size() + 1) * (128 + frame_plane_spacing.x) + frame_plane_spacing.x;

				float ratio = last_size / size;

				scrollbar_possition.x = scrollbar_possition.x * ratio;
			}

			void remove_selected ( ) {

				if(current_frame_id != -1) {

					sprite_sheet->erase(sprite_sheet->begin() + current_frame_id);

					float old_size = size;

					size = size - 128 - frame_plane_spacing.x;

					if(size < 1)

						size = 1;

					float ratio = old_size / size;

					scrollbar_possition.x = scrollbar_possition.x / ratio;

					current_frame_id = -1;

					scroll(0);
				}
			}

			enum CopyBehavior {

				CB_NONE,

				CB_COPY,
				CB_CUT,
			};

			bool copy_selected (  ) {

				if(current_frame_id != -1) {

					copy = current_object->frames[current_frame_id];

					is_copy = CopyBehavior::CB_COPY;

					return true;
				}

				return false;
			}

			bool cut_selected ( ) {

				if(current_frame_id != -1) {

					copy = current_object->frames[current_frame_id];

					remove_selected();

					is_copy = CopyBehavior::CB_CUT;

					return true;
				}

				return false;
			}

			bool paste_after_selected ( ) {

				if(is_copy == CopyBehavior::CB_NONE)

					return false;

				else if(is_copy == CopyBehavior::CB_COPY) {

					if(current_frame_id != -1) {

						add_frame(copy,current_object->frames.begin() + current_frame_id);

						return true;
					}
				}

				else if(is_copy == CopyBehavior::CB_CUT) {

					if(current_frame_id != -1) {

						add_frame(copy,current_object->frames.begin() + current_frame_id);

						is_copy = CopyBehavior::CB_NONE;

						return true;
					}
				}

				return false;
			}

			int is_copy;

			Plane copy;

			int current_frame_id;

			Vector2F scrollbar_possition;

			SpriteSheet * sprite_sheet;

			Vector2F draw_offset;

			Vector2F frame_plane_spacing;

			Plane plane;

			float size;
		};

		FrameList frame_list;

		class UV_editor {

			public :

			UV_editor ( ) {

				plane = Plane(uv_window_pos,Vector2F(float(uv_window_width),float(uv_window_height)) + uv_window_pos);

				size = Vector2F(100,100);

				texture_plane = Plane(Vector2F(0,0),size);

				texture_plane_pos = (Vector2F(float(uv_window_width),float(uv_window_height)) - size) / 2 + uv_window_pos;

				scale = Vector2F(1,1);

				current_drag_id = -1;

				remainder = Vector2F(0,0);
			}

			void draw ( ) {

				glLineWidth(2);

				glBegin(GL_LINES);

					glColor3f(0.6f,0.6f,0.6f);

					glVertex2f(uv_window_pos.x,float(uv_window_height + 1));
					glVertex2f(uv_window_pos.x + float(uv_window_width),float(uv_window_height + 1));

				glEnd();

				glEnable(GL_SCISSOR_TEST);

				glScissor(int(uv_window_pos.x),int(window_height) - uv_window_height,uv_window_width,uv_window_height);

				bind_texture(0);

				Plane texture_plane_with_pos = texture_plane + texture_plane_pos;

				/* draw texture quads */
				glBegin(GL_QUADS);

					glColor3f(1.0f,1.0f,1.0f);
					glVertex2f(texture_plane_with_pos.points[0].x,texture_plane_with_pos.points[0].y);
					glVertex2f(texture_plane_with_pos.points[1].x,texture_plane_with_pos.points[1].y);
					glVertex2f(texture_plane_with_pos.points[2].x,texture_plane_with_pos.points[2].y);
					glVertex2f(texture_plane_with_pos.points[3].x,texture_plane_with_pos.points[3].y);

				glEnd();
				
				if(texture_list.current_texture != 0) {

					bind_texture(texture_list.current_texture);

					/* draw texture quads */
					glBegin(GL_QUADS);

					glColor3f(1.0f,1.0f,1.0f);

					glTexCoord2f(0,0);
					glVertex2f(texture_plane_with_pos.points[0].x,texture_plane_with_pos.points[0].y);

					glTexCoord2f(1,0);
					glVertex2f(texture_plane_with_pos.points[1].x,texture_plane_with_pos.points[1].y);

					glTexCoord2f(1,1);
					glVertex2f(texture_plane_with_pos.points[2].x,texture_plane_with_pos.points[2].y);

					glTexCoord2f(0,1);
					glVertex2f(texture_plane_with_pos.points[3].x,texture_plane_with_pos.points[3].y);

					glEnd();

					bind_texture(0);
				}

				/* draw masking rect */

				if(frame_list.current_frame_id != -1) {

					Plane uv_plane = current_object->frames[frame_list.current_frame_id];

					Plane texture_plane_00 = texture_plane_with_pos - texture_plane_with_pos.points[0];

					Plane mask_plane = Plane(

						texture_plane_00.points[2] * uv_plane.points[0],
						texture_plane_00.points[2] * uv_plane.points[1],
						texture_plane_00.points[2] * uv_plane.points[2],
						texture_plane_00.points[2] * uv_plane.points[3]

					) + texture_plane_with_pos.points[0];

					glBegin(GL_QUADS);

						glColor4f(1.0f,0.0f,1.0f,0.5f);

						glVertex2f(mask_plane.points[0].x,mask_plane.points[0].y);
						glVertex2f(mask_plane.points[1].x,mask_plane.points[1].y);
						glVertex2f(mask_plane.points[2].x,mask_plane.points[2].y);
						glVertex2f(mask_plane.points[3].x,mask_plane.points[3].y);

					glEnd();

					/* draw points */

					glPointSize(10);

					glBegin(GL_POINTS);

						glColor3f(0.0f,1.0f,1.0f);

						glVertex2f(mask_plane.points[0].x,mask_plane.points[0].y);
						glVertex2f(mask_plane.points[1].x,mask_plane.points[1].y);
						glVertex2f(mask_plane.points[2].x,mask_plane.points[2].y);
						glVertex2f(mask_plane.points[3].x,mask_plane.points[3].y);

					glEnd();
				}

				glDisable(GL_SCISSOR_TEST);
			}

			void zoom(int32_t const & z) {

				/* find where the middle of uv_window mets texture_plane */
				Vector2F middle = (Vector2F(float(uv_window_width),float(uv_window_height)) / 2 + uv_window_pos) - texture_plane_pos;

				Plane old_plane = texture_plane;

				texture_plane = texture_plane - middle;

				if(z > 0) {

					texture_plane = texture_plane.scaled(1.1f * z) + middle;

					scale = scale * Vector2F(1.1f * z);
				}

				else if(z < 0) {

					texture_plane = texture_plane / Vector2F(-1.1f * z) + middle;

					scale = scale / Vector2F(-1.1f * z);
				}
			}

			void drag ( Vector2F const & rel ) {

				texture_plane_pos = texture_plane_pos + rel;
			}

			int32_t is_on_point ( Vector2F const & mouse_xy ) {

				if(frame_list.current_frame_id == -1)

					return -1;

				Plane uv_plane = current_object->frames[frame_list.current_frame_id];

				Plane texture_plane_with_pos = texture_plane + texture_plane_pos;

				Plane texture_plane_00 = texture_plane_with_pos - texture_plane_with_pos.points[0];

				Plane world_space_uv = Plane(

					texture_plane_00.points[2] * uv_plane.points[0],
					texture_plane_00.points[2] * uv_plane.points[1],
					texture_plane_00.points[2] * uv_plane.points[2],
					texture_plane_00.points[2] * uv_plane.points[3]

				) + texture_plane_with_pos.points[0] - mouse_xy;

				for(unsigned int i = 0; i < 4; i++) {

					if(world_space_uv.points[i].length2() < 20)

						return i;
				}

				return -1;
			}

			void drag_point ( int32_t const & point, Vector2F const & mouse_rel ) {

				if(point != -1)

					if(current_object->frames.size() != 0)

						if(frame_list.current_frame_id != -1) {

							Plane uv_plane = current_object->frames[frame_list.current_frame_id];

							Plane texture_plane_with_pos = texture_plane + texture_plane_pos;

							Plane texture_plane_00 = texture_plane_with_pos - texture_plane_with_pos.points[0];

							Vector2F world_space_uv = texture_plane_00.points[2] * uv_plane.points[point] + mouse_rel;

							Vector2F new_uv = world_space_uv / texture_plane_00.points[2];

							Vector2F pixel_uv = Vector2F(

								1 / texture_list.textures[texture_list.current_id].wh.x,
								1 / texture_list.textures[texture_list.current_id].wh.y
							);

							Vector2F difference = (new_uv - uv_plane.points[point]) + remainder;

							Vector2F ratio = (difference / pixel_uv);

							Vector2F rounded = Vector2F(std::floor(ratio.x),std::floor(ratio.y)) * pixel_uv;

							remainder = difference - rounded;



							current_object->frames[frame_list.current_frame_id].points[point] += rounded;
						}
			}

			Plane get_mask_plane ( ) {

				if(frame_list.current_frame_id != -1) {

					Plane uv_plane = current_object->frames[frame_list.current_frame_id];

					Plane texture_plane_with_pos = texture_plane + texture_plane_pos;

					Plane texture_plane_00 = texture_plane_with_pos - texture_plane_with_pos.points[0];

					Plane world_space_uv = Plane(

						texture_plane_00.points[2] * uv_plane.points[0],
						texture_plane_00.points[2] * uv_plane.points[1],
						texture_plane_00.points[2] * uv_plane.points[2],
						texture_plane_00.points[2] * uv_plane.points[3]

					) + texture_plane_with_pos.points[0];

					return world_space_uv;
				}

				return Plane();
			}

			void drag_mask ( Vector2F const & mouse_rel ) {

				Vector2F cpy_rem = remainder;

				remainder = cpy_rem;
				drag_point(0,mouse_rel);

				remainder = cpy_rem;
				drag_point(1,mouse_rel);

				remainder = cpy_rem;
				drag_point(2,mouse_rel);

				remainder = cpy_rem;
				drag_point(3,mouse_rel);

				//remainder = cpy_rem;
			}

			Vector2F remainder;
			int32_t current_drag_id;
			Vector2F scale;
			Plane plane;
			Plane texture_plane;
			Vector2F texture_plane_pos;
			Vector2F size;
		};

		UV_editor uv_editor;

		class PreviewWindow {

			public :

			void init ( ) {

				window_plane = Plane(Vector2F(0,0),Vector2F(128,128));

				plane = Plane(preview_window_pos,Vector2F(float(preview_window_width),float(preview_window_height)) + preview_window_pos);
			}

			void draw ( ) {
			
				glBegin(GL_QUADS);

					glColor3f(1.0f,1.0f,1.0f);

					glVertex2f(plane.points[0].x,plane.points[0].y);
					glVertex2f(plane.points[1].x,plane.points[1].y);
					glVertex2f(plane.points[2].x,plane.points[2].y);
					glVertex2f(plane.points[3].x,plane.points[3].y);

				glEnd();

				if(texture_list.current_id != -1) {

					if(current_object->frames.size() != 0) {

						bind_texture(texture_list.current_texture);

						glBegin(GL_QUADS);

						Plane current_frame = current_object->get_frame();

						glColor3f(1.0f,1.0f,1.0f);

							glTexCoord2f(current_frame.points[0].x,current_frame.points[0].y);
							glVertex2f(plane.points[0].x,plane.points[0].y);

							glTexCoord2f(current_frame.points[1].x,current_frame.points[1].y);
							glVertex2f(plane.points[1].x,plane.points[1].y);

							glTexCoord2f(current_frame.points[2].x,current_frame.points[2].y);
							glVertex2f(plane.points[2].x,plane.points[2].y);

							glTexCoord2f(current_frame.points[3].x,current_frame.points[3].y);
							glVertex2f(plane.points[3].x,plane.points[3].y);

						glEnd();

						bind_texture(0);
					}
				}
			}

			void inc_speed ( int const & speed ) {

				if(current_object->frames.size() != 0) {

					/* calculate how long it takes to show one frame */
					uint32_t frame_time = current_object->run_time / current_object->frames.size();

					if(frame_time == 0)

						frame_time = 1;

					/* calculate how long animation had been running */
					uint32_t const animation_run_time = _GET_UTC_TIME_MS_ - current_object->start_time;

					int32_t current_frame = animation_run_time / frame_time;

					current_object->run_time = current_object->run_time + frame_time * (-speed);

					current_object->set_current_frame(current_frame);
				}
			}

			Vector2F window_plane_possition;

			Plane window_plane;

			Plane plane;
		};

		PreviewWindow preview_window;


		int32_t animation_editor_callback ( Window * window, Event const & event) {

			if(event.type == SDL_WINDOWEVENT) {

				if(event.sdl_event.window.event == SDL_WINDOWEVENT_CLOSE) {

					Application::destroy_window(window);
				}
			}

			else if(event.type == SDL_DROPFILE) {

				Texture tex;

				tex.texture = load_texture(event.sdl_event.drop.file,tex.wh);

				texture_list.add(tex);

				SDL_free(event.sdl_event.drop.file);
			}

			else if(event.type == SDL_MOUSEMOTION) {

				Vector2F mouse_xy = Vector2F(float(event.sdl_event.motion.x),float(event.sdl_event.motion.y));

				if(current_action == EditorAction::TEXTURE_LIST_SCROLLING)

					texture_list.scroll_scrollbar(event.sdl_event.motion.yrel);

				else if(current_action == EditorAction::UV_EDITOR_DRAG) {

					uv_editor.drag(Vector2F(float(event.sdl_event.motion.xrel),float(event.sdl_event.motion.yrel)));
				}

				else if(current_action == EditorAction::UV_EDITOR_DRAG_POINT) {

					uv_editor.drag_point(uv_editor.current_drag_id,Vector2F(float(event.sdl_event.motion.xrel),float(event.sdl_event.motion.yrel)));
				}

				else if(current_action == EditorAction::UV_EDITOR_MASK_DRAG) {

					uv_editor.drag_mask(Vector2F(float(event.sdl_event.motion.xrel),float(event.sdl_event.motion.yrel)));
				}

				else if(current_action == EditorAction::FRAME_LIST_SCROLLING) {

					frame_list.scroll_scrollbar(event.sdl_event.motion.xrel);
				}

				else if(current_action & EditorAction::FOCUS) {

					if(texture_list.plane.aligned_collides_point(mouse_xy)) {

						current_action = EditorAction::TEXTURE_LIST_FOCUS;
					}

					else if(uv_editor.plane.aligned_collides_point(mouse_xy)) {

						current_action = EditorAction::UV_EDITOR_FOCUS;
					}

					else if(frame_list.plane.aligned_collides_point(mouse_xy)) {

						current_action = EditorAction::FRAME_LIST_FOCUS;
					}

					else if(preview_window.plane.aligned_collides_point(mouse_xy)) {

						current_action = EditorAction::PREVIEW_WINDOW_FOCUS;
					}
				}
			}

			else if(event.type == SDL_MOUSEWHEEL) {

				if(current_action == animation_editor::EditorAction::TEXTURE_LIST_FOCUS)

					texture_list.scroll(event.sdl_event.wheel.y);

				else if(current_action == animation_editor::EditorAction::UV_EDITOR_FOCUS)

					uv_editor.zoom(event.sdl_event.wheel.y);

				else if(current_action == animation_editor::EditorAction::FRAME_LIST_FOCUS)

					frame_list.scroll(event.sdl_event.wheel.y);

				else if(current_action == animation_editor::EditorAction::PREVIEW_WINDOW_FOCUS)

					preview_window.inc_speed(event.sdl_event.wheel.y);
			}

			else if(event.type == SDL_MOUSEBUTTONDOWN) {

				Vector2F mouse_xy = Vector2F((float)event.sdl_event.button.x,(float)event.sdl_event.button.y);

				if(event.sdl_event.button.button == SDL_BUTTON_LEFT) {

					uv_editor.current_drag_id = uv_editor.is_on_point(mouse_xy);

					if(uv_editor.current_drag_id != -1) {

						current_action = EditorAction::UV_EDITOR_DRAG_POINT;
					}

					else if(texture_list.plane.aligned_collides_point(mouse_xy)) {

						current_action = EditorAction::TEXTURE_LIST_FOCUS;

						if(!texture_list.select_texture(mouse_xy))

							if(texture_list.is_on_scrollbar(mouse_xy))

								current_action = EditorAction::TEXTURE_LIST_SCROLLING;
					}

					else if(uv_editor.get_mask_plane().aligned_collides_point(mouse_xy)) {

						current_action = EditorAction::UV_EDITOR_MASK_DRAG;
					}

					else if(uv_editor.plane.aligned_collides_point(mouse_xy)) {

						current_action = EditorAction::UV_EDITOR_FOCUS;
					}

					else if(frame_list.plane.aligned_collides_point(mouse_xy)) {

						current_action = EditorAction::FRAME_LIST_FOCUS;

						if(!frame_list.select_frame(mouse_xy))

							if(frame_list.is_on_scrollbar(mouse_xy))

								current_action = EditorAction::FRAME_LIST_SCROLLING;
					}
				}

				else if(event.sdl_event.button.button == SDL_BUTTON_RIGHT) {

					if((uv_editor.texture_plane + uv_editor.texture_plane_pos).aligned_collides_point(mouse_xy)) {

						current_action = EditorAction::UV_EDITOR_DRAG;
					}
				}
			}

			else if(event.type == SDL_MOUSEBUTTONUP) {

				if(current_action == EditorAction::TEXTURE_LIST_SCROLLING)

					current_action = EditorAction::TEXTURE_LIST_FOCUS;

				else if(current_action == EditorAction::UV_EDITOR_DRAG)

					current_action = EditorAction::UV_EDITOR_FOCUS;

				else if(current_action == EditorAction::FRAME_LIST_SCROLLING)

					current_action = EditorAction::FRAME_LIST_FOCUS;

				else if(current_action == EditorAction::UV_EDITOR_DRAG_POINT)

					current_action = EditorAction::UV_EDITOR_FOCUS;

				else if(current_action == EditorAction::UV_EDITOR_MASK_DRAG)

					current_action = EditorAction::UV_EDITOR_FOCUS;
			}

			else if(event.type == SDL_KEYDOWN) {

				if(event.sdl_event.key.keysym.scancode == SDL_SCANCODE_DELETE) {

					if(current_action == EditorAction::TEXTURE_LIST_FOCUS)

						texture_list.remove_selected();

					else if(current_action == EditorAction::FRAME_LIST_FOCUS)

						frame_list.remove_selected();
				}

				else if(event.sdl_event.key.keysym.scancode == SDL_SCANCODE_C) {

					if(event.sdl_event.key.keysym.mod & KMOD_CTRL)

						frame_list.copy_selected();
				}

				else if(event.sdl_event.key.keysym.scancode == SDL_SCANCODE_X) {

					if(event.sdl_event.key.keysym.mod & KMOD_CTRL)

						frame_list.cut_selected();
				}

				else if(event.sdl_event.key.keysym.scancode == SDL_SCANCODE_V) {

					if(event.sdl_event.key.keysym.mod & KMOD_CTRL)

						frame_list.paste_after_selected();
				}
			}

			else if(event.type == ZM_WINDOW_EVENT) {

				if(event.zm_event.window.event == ZM_WINDOW_EVENT_DESTROY) {

					Animation::editing = false;
				}

				else if(event.zm_event.window.event == ZM_WINDOW_EVENT_RENDER) {

					window->make_current();

					glClear(GL_COLOR_BUFFER_BIT);

					texture_list.draw();

					frame_list.draw();

					uv_editor.draw();

					preview_window.draw();

					SDL_GL_SwapWindow(window->handle);

					post_rendering_message(window);
				}
			}

			return 0;
		}

	} /* namespace animation_editor */

	bool Animation::edit ( Window * parent ) {

		if(Animation::editing == false) {

			animation_editor::frame_list.sprite_sheet = &(this)->frames;

			Animation::editing = true;

			Window * window = Application::create_window("Animation editor",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,1440,800,0,parent,animation_editor::animation_editor_callback);

			post_rendering_message(window);

			/* make a coppy of this */
			animation_editor::current_object = (this);

			animation_editor::frame_list.init();

			animation_editor::preview_window.init();

			glMatrixMode( GL_PROJECTION );
			glLoadIdentity( );

			glOrtho(0,1440,800,0,0,1.0);

			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();

			glClearColor(0.9f,0.9f,0.9f,1);

			glEnable(GL_TEXTURE_2D);

			glDisable(GL_DEPTH_TEST);

			glEnable(GL_BLEND);

			glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

			animation_editor::Texture tex;

			tex.texture = animation_editor::load_texture("sample1.png",tex.wh);

			animation_editor::texture_list.add(tex);

			tex.texture = animation_editor::load_texture("sample2.png",tex.wh);

			animation_editor::texture_list.add(tex);

			return true;
		}

		return false;
	}

} /* namespace zm */ 

#endif /* #ifndef _ANIMATION_HPP_ */