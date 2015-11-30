#include "graphics3d.h"
#include "simple_logger.h"
#include "shader.h"
//#include <GL/glu.h>
#include <glm/gtc/matrix_transform.hpp>
#include "game.h"
#include <stdexcept>


void graphics3d_close();

GLuint graphics3d_get_shader_program()
{
	if(game.shader != -1)
		return ShaderPrograms[game.shader].id;
	else
		return 0;
}

void graphics3d_setup_default_light();

int graphics3d_init(int sw, int sh, int fullscreen, const char* project, Uint32 frameDelay)
{
	const unsigned char* version;
	GLenum glew_status;
	ShaderProgram *shader;
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		slog("failed to initialize SDL!");
		return -1;
	}
	atexit(SDL_Quit);
	game.frame_delay = frameDelay;

	game.window = SDL_CreateWindow(project ? project : "gametest3d",
	                               SDL_WINDOWPOS_UNDEFINED,
	                               SDL_WINDOWPOS_UNDEFINED,
	                               sw, sh,
	                               SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);


	game.context = SDL_GL_CreateContext(game.window);
	if (game.context == NULL)
	{
		slog("There was an error creating the OpenGL context!\n");
		return -1;
	}

	version = glGetString(GL_VERSION);
	if (version == NULL)
	{
		slog("There was an error creating the OpenGL context!\n");
		return -1;
	}

	SDL_GL_MakeCurrent(game.window, game.context);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

	//MUST make a context AND make it current BEFORE glewInit()!
	glewExperimental = GL_TRUE;
	glew_status = glewInit();
	if (glew_status != 0)
	{
		slog("Error: %s", glewGetErrorString(glew_status));
		return -1;
	}

	//Load the various shaders
	shader = BuildShaderProgram("shaders/vs1.glsl", "shaders/fs1.glsl", SHADERPROG_BASE);
	game.shader = SHADERPROG_BASE;
	//Proximity Shader
	shader = BuildShaderProgram("shaders/vs1.glsl", "shaders/proximity_frag.glsl", SHADERPROG_PROXIMITY);
	ShaderPrograms[SHADERPROG_PROXIMITY].uniforms[0] = glGetUniformLocation(ShaderPrograms[SHADERPROG_PROXIMITY].id, "Proximity_color");
	ShaderPrograms[SHADERPROG_PROXIMITY].uniforms[1] = glGetUniformLocation(ShaderPrograms[SHADERPROG_PROXIMITY].id, "Proximity_unit");
	shader->setup = SetupProximityShader;
	shader->reset = ResetProximityShader;
	(*shader->reset)();
	//Wireframe Shader
	shader = BuildShaderProgram("shaders/vs1.glsl", "shaders/wireframe_frag.glsl", SHADERPROG_WIREFRAME);
	ShaderPrograms[SHADERPROG_WIREFRAME].uniforms[0] = glGetUniformLocation(ShaderPrograms[SHADERPROG_WIREFRAME].id, "Wireframe_color");
	ShaderPrograms[SHADERPROG_WIREFRAME].uniforms[1] = glGetUniformLocation(ShaderPrograms[SHADERPROG_WIREFRAME].id, "Wireframe_thickness");
	shader->setup = SetupWireframeShader;
	shader->reset = ResetWireframeShader;
	(*shader->reset)();
	//Outline Shader
	shader = BuildShaderProgram("shaders/outline_vert.glsl", "shaders/outline_frag.glsl", SHADERPROG_OUTLINE);
	ShaderPrograms[SHADERPROG_OUTLINE].uniforms[0] = glGetUniformLocation(ShaderPrograms[SHADERPROG_OUTLINE].id, "Outline_color");
	ShaderPrograms[SHADERPROG_OUTLINE].uniforms[1] = glGetUniformLocation(ShaderPrograms[SHADERPROG_OUTLINE].id, "Outline_unit");
	//Infrared Shader
	//shader = BuildShaderProgram("shaders/infrared_vert.glsl", "shaders/infrared_frag.glsl", SHADERPROG_INFRARED);
	//ShaderPrograms[SHADERPROG_PROXIMITY].uniforms[0] = glGetUniformLocation(ShaderPrograms[SHADERPROG_INFRARED].id, "Infrared_color");
	//ShaderPrograms[SHADERPROG_PROXIMITY].uniforms[1] = glGetUniformLocation(ShaderPrograms[SHADERPROG_INFRARED].id, "Background_color");


	atexit(graphics3d_close);

	//Generate the buffer set
	glGenBuffers(1, &game.vertexbuffer);
	glGenBuffers(1, &game.UVbuffer);
	glGenBuffers(1, &game.normalbuffer);
	glGenBuffers(1, &game.barybuffer);
	
	game.Projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 1000.0f);
	graphics3d_setup_default_light();
	return 0;
}

void graphics3d_close()
{
	SDL_GL_DeleteContext(game.context);
}

void graphics3d_frame_begin()
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glLoadIdentity();
	//glPushMatrix();
}

void graphics3d_next_frame()
{
	static Uint32 then = 0;
	Uint32 now;
	//glPopMatrix();
	SDL_GL_SwapWindow(game.window);
	now = SDL_GetTicks();
	if ((now - then) < game.frame_delay)
	{
		SDL_Delay(game.frame_delay - (now - then));
	}
	//slog("ticks passed this frame: %i",(now - then));
	then = now;
}

void graphics3d_setup_default_light()
{
	GLfloat mat_specular[] = {1.0f, 1.0f, 1.0f, 1.0f};
	GLfloat mat_shininess[] = {50.0f};
	GLfloat light_position[] = {-10.0f, -10.0f, 10.0f, 0.0f};
	GLfloat light_ambient[] = {1.0f, 1.0f, 1.0f, 1.0f};
	GLfloat light_diffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
	GLfloat light_specular[] = {1.0f, 1.0f, 1.0f, 1.0f};

	GLfloat light1_ambient[] = {1.2f, 1.2f, 1.2f, 1.0f};
	GLfloat light1_diffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
	GLfloat light1_specular[] = {1.0f, 1.0f, 1.0f, 1.0f};
	GLfloat light1_position[] = {-20.0f, 2.0f, 1.0f, 1.0f};
	GLfloat spot_direction[] = {-1.0f, -1.0f, 0.0f};

	GLfloat lmodel_ambient[] = {0.4f, 0.4f, 0.4f, 1.0f};

	

	//glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);

	////glShadeModel (GL_SMOOTH);

	//glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	//glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	//glLightfv(GL_LIGHT0, GL_POSITION, light_position);


	//glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	//glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	//glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	//glLightfv(GL_LIGHT0, GL_POSITION, light_position);


	//glLightfv(GL_LIGHT1, GL_AMBIENT, light1_ambient);
	//glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
	//glLightfv(GL_LIGHT1, GL_SPECULAR, light1_specular);
	//glLightfv(GL_LIGHT1, GL_POSITION, light1_position);
	//glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 1.9);
	//glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.7);
	//glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.3);

	//glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 80.0);
	//glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, spot_direction);
	//glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 2.0);

	//glEnable(GL_LIGHTING);
	//glEnable(GL_LIGHT0);
	//glEnable(GL_LIGHT1);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_BLEND);
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);
}


/*eol@eof*/

