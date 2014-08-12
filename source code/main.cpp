#include "texture.hpp"
#include "sprite.hpp"
#include "animation.hpp"
#include "lin_alg.hpp"

void draw ( zm::Plane const & plane, GLuint const & texture) {

	zm::bind_texture(texture);

	glBegin(GL_QUADS);

		glColor3f(1.0f,1.0f,1.0f);

		glTexCoord2f(0,0); glVertex2f(plane.points[0].x,plane.points[0].y);

		glTexCoord2f(0,1); glVertex2f(plane.points[1].x,plane.points[1].y);

		glTexCoord2f(1,1); glVertex2f(plane.points[2].x,plane.points[2].y);

		glTexCoord2f(1,0); glVertex2f(plane.points[3].x,plane.points[3].y);

	glEnd();
}

void draw ( zm::Plane const & plane, zm::Sprite const & sprite, GLuint const & texture) {

	zm::bind_texture(texture);

	glBegin(GL_QUADS);

		glColor3f(1.0f,1.0f,1.0f);

		glTexCoord2f(sprite.points[0].u,sprite.points[0].v); glVertex2f(plane.points[0].x,plane.points[0].y);

		glTexCoord2f(sprite.points[1].u,sprite.points[1].v); glVertex2f(plane.points[1].x,plane.points[1].y);

		glTexCoord2f(sprite.points[2].u,sprite.points[2].v); glVertex2f(plane.points[2].x,plane.points[2].y);

		glTexCoord2f(sprite.points[3].u,sprite.points[3].v); glVertex2f(plane.points[3].x,plane.points[3].y);

	glEnd();
}



int main ( int argc, char ** argv ) {

	SDL_Init(SDL_INIT_EVERYTHING);

	if(TTF_Init() == -1)

		throw std::runtime_error("lel");

	SDL_Window * window = SDL_CreateWindow("ZORDZMAN",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,800,600,SDL_WINDOW_OPENGL);

	SDL_GLContext context = SDL_GL_CreateContext(window);

	SDL_GL_MakeCurrent(window,context);

	glMatrixMode( GL_PROJECTION );
    glLoadIdentity( );

	glOrtho(0,800,600,0,0,1.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glClearColor(1,1,1,1);

	glEnable(GL_TEXTURE_2D);

	glDisable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);

	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);


	GLuint tex = zm::load_texture("dude_animation_sheet-480x480.png");

	zm::Plane plane(zm::Vector2F(200,200),zm::Vector2F(400,350));

	zm::Animation anim;

	anim.is_looping = true;

	anim.def_frame = 0;

	anim.run_time = 1000;

	/* generate animation frames */
	anim.generate_frames(zm::Vector2U(480,480),zm::Vector2U(0,0),zm::Vector2U(61,70),zm::Vector2U(0,0),zm::Vector2U(7,4),27);

	anim.start();

	anim.store("test.anim"); 
	
	anim.clean();
	
	anim.load("test.anim");

	SDL_Event event;

	bool application_is_running = true;

	while(application_is_running) {

		while(SDL_PollEvent(&event)) {

			if(event.type == SDL_WINDOWEVENT) {

				if(event.window.event == SDL_WINDOWEVENT_CLOSE) {

					application_is_running = false;
				}
			}
		}

		glClear(GL_COLOR_BUFFER_BIT);

		draw(plane,anim.get_frame(),tex);

		SDL_GL_SwapWindow(window);
	}

	//GLEW_quit();
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();

	return 0;
}