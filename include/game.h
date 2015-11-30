
#ifndef __GAME_H //Game.h
#define __GAME_H
#include "shader.h"

typedef struct GameEngine_S
{
	glm::mat4 Projection;
	glm::mat4 View;
	glm::vec3 light_position;
	GLuint vertexbuffer;
	GLuint UVbuffer;
	GLuint normalbuffer;
	GLuint barybuffer;
	GLuint shader;
	SDL_Window *window;
	SDL_GLContext context;
	Uint32 frame_delay;
	GLuint impulse_flags;
	GLuint graphics_flags;
}GameEngine;


extern GameEngine game;
//Flag setters
#define IS_SET(a, b) (a & b)
#define SET_FLAG(a, b) (a |= b)
#define REMOVE_FLAG(a, b) (a &= ~b)
#define TOGGLE_FLAG(a, b) (a ^= b)

//Impulse flags for handling complex inputs
#define IMPFLAG_SHADER_MOD		1
#define IMPFLAG_GRAPHICS_MOD	2

//Graphics flags for handling special graphics settings
#define GFXFLAG_WIREFRAME	1
#define GFXFLAG_OUTLINE		2

#endif __GAME_H