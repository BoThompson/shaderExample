#ifndef __SHADER_H__
#define __SHADER_H__
/**
 * shader
 * @license The MIT License (MIT)
   @copyright Copyright (c) 2015 EngineerOfLies
    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:
    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.
    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
 */
#include <GL/glew.h>

#define MAX_UNIFORMS 10
#define MAX_VALUES MAX_UNIFORMS * 16 //Enough for a 4x4 matrix per uniform if necessary
#define MAX_SHADERS 16

typedef struct ShaderProgram_S
{
	GLuint id;
	GLuint MVP_id;
	GLuint Model_id;
	GLuint View_id;
	GLuint light_position_id;
	GLuint complexity;
	GLuint max_complexity;
	GLuint uniforms[MAX_UNIFORMS];
	GLfloat values[MAX_VALUES];
	void (*setup)();
	void (*reset)();
}ShaderProgram;


ShaderProgram *BuildShaderProgram(const char *vsPath, const char *fsPath);
ShaderProgram *BuildShaderProgram(const char *vsPath, const char *fsPath, GLuint shader_num);
GLuint CreateShader(GLenum eShaderType, const char *strShaderFile);
void RunShader();

#define SHADERPROG_BASE			0
#define SHADERPROG_WIREFRAME	1
#define SHADERPROG_PROXIMITY	2
#define SHADERPROG_TOON			3
#define SHADERPROG_PULSE		4
#define SHADERPROG_OUTLINE		5
#define SHADERPROG_INFRARED		6


//Uniforms / Values
/*********************************************************************
 * Proximity shader:
 *
 * 0-2 Proximity color
 *   3 Proximity thickness
 *********************************************************************/
#define PROXSHADER_COLOR		0
#define PROXSHADER_DISTANCE		1
void SetupProximityShader();
void ResetProximityShader();
/*********************************************************************
 * Outline shader:
 *
 * 0-2 Outline color
 *   3 Outline thickness
 *********************************************************************/
#define OUTLNSHADER_COLOR		0
#define OUTLNSHADER_DISTANCE	1

/*********************************************************************
 * Wireframe shader:
 *
 * 0-2 Wireframe color
 *   3 Wireframe thickness
 *********************************************************************/
#define WIRESHADER_COLOR		0
#define WIRESHADER_DISTANCE		1
void SetupWireframeShader();
void ResetWireframeShader();


extern ShaderProgram ShaderPrograms[MAX_SHADERS];





#endif