#include "graphics3d.h"
#include "simple_logger.h"
#include "shader.h"
//#include <GL/glu.h>
#include <glm/gtc/matrix_transform.hpp>
#include <stdio.h>
#include <game.h>

ShaderProgram ShaderPrograms[MAX_SHADERS];
GLuint shader_count = 0;


//Setup Shader Function Callbacks
void SetupProximityShader()
{
	glUniform3fv(ShaderPrograms[SHADERPROG_PROXIMITY].uniforms[PROXSHADER_COLOR],1,
				 ShaderPrograms[SHADERPROG_PROXIMITY].values);
	glUniform1f(ShaderPrograms[SHADERPROG_PROXIMITY].uniforms[PROXSHADER_DISTANCE],
				ShaderPrograms[SHADERPROG_PROXIMITY].values[3]);
}

void SetupWireframeShader()
{
	glUniform3fv(ShaderPrograms[SHADERPROG_WIREFRAME].uniforms[WIRESHADER_COLOR], 1,
				 ShaderPrograms[SHADERPROG_WIREFRAME].values);
	glUniform1f(ShaderPrograms[SHADERPROG_WIREFRAME].uniforms[WIRESHADER_DISTANCE],
				ShaderPrograms[SHADERPROG_WIREFRAME].values[3]);
}

//Reset Shader Function Callbacks
void ResetProximityShader()
{
	
	ShaderPrograms[SHADERPROG_PROXIMITY].values[0] = 0.0f;
	ShaderPrograms[SHADERPROG_PROXIMITY].values[1] = 1.0f;
	ShaderPrograms[SHADERPROG_PROXIMITY].values[2] = 0.0f;
	ShaderPrograms[SHADERPROG_PROXIMITY].values[3] = 75.0f;
}

void ResetWireframeShader()
{
	
	ShaderPrograms[SHADERPROG_WIREFRAME].values[0] = 0.0f;
	ShaderPrograms[SHADERPROG_WIREFRAME].values[1] = 0.0f;
	ShaderPrograms[SHADERPROG_WIREFRAME].values[2] = 0.0f;
	ShaderPrograms[SHADERPROG_WIREFRAME].values[3] = 0.05f;
}


ShaderProgram *BuildShaderProgram(const char *vsPath, const char *fsPath)
{
	return BuildShaderProgram(vsPath, fsPath, shader_count++);
}
ShaderProgram *BuildShaderProgram(const char *vsPath, const char *fsPath, GLuint shader_num)
{
    GLint infoLogLength;
    GLuint vertexShader;
    GLuint fragmentShader;
    GLuint tempProgram;
    GLint status;
    GLchar strInfoLog[4096];
    ShaderProgram *shader;
    vertexShader = CreateShader(GL_VERTEX_SHADER, vsPath);
    fragmentShader = CreateShader(GL_FRAGMENT_SHADER, fsPath);
    
    tempProgram = glCreateProgram();
    glAttachShader(tempProgram, vertexShader);
    glAttachShader(tempProgram, fragmentShader);
    
    glLinkProgram(tempProgram);
    
    glGetProgramiv(tempProgram, GL_LINK_STATUS, &status);
    if (status == GL_FALSE)
    {
        glGetProgramiv(tempProgram, GL_INFO_LOG_LENGTH, &infoLogLength);
        
        glGetProgramInfoLog(tempProgram, infoLogLength, NULL, strInfoLog);
        
        slog("Shader linker failure: %s", strInfoLog);
        return NULL;
    }
    
    glDetachShader(tempProgram, vertexShader);
    glDetachShader(tempProgram, fragmentShader);
    shader = &ShaderPrograms[shader_num];
	//Set up the various shader specific ids
	shader->id				  = tempProgram;
	shader->MVP_id			  = glGetUniformLocation(shader->id, "MVP");
	shader->Model_id		  = glGetUniformLocation(shader->id, "M");
	shader->View_id			  = glGetUniformLocation(shader->id, "V");
	shader->light_position_id = glGetUniformLocation(shader->id, "LightPosition_worldspace");
	shader->setup			  = NULL;
	shader->reset			  = NULL;
    return shader;
}

GLuint CreateShader(GLenum eShaderType, const char *strShaderFile)
{
    char shaderSource[4096];
    char inChar;
    FILE *shaderFile;
    int i = 0;
    GLuint shader;
    GLint status;
    GLint infoLogLength;
    GLchar strInfoLog[4096];
    char strShaderType[16];
    const char *ss;
    
    shaderFile = fopen(strShaderFile, "r");
    if (!shaderFile)
    {
        slog("failed to open shader file: %s",strShaderFile);
        return 0;
    }
    while(fscanf(shaderFile,"%c",&inChar) > 0)
    {
        shaderSource[i++] = inChar; //loading the file's chars into array
    }
    shaderSource[i - 1] = '\0';
    fclose(shaderFile);
    
    shader = glCreateShader(eShaderType);
    ss = shaderSource;
    glShaderSource(shader, 1, &ss, NULL);
    
    glCompileShader(shader);
    
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE)
    {
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
        
        glGetShaderInfoLog(shader, infoLogLength, NULL, strInfoLog);
        
        switch(eShaderType)
        {
            case GL_VERTEX_SHADER:
                sprintf(strShaderType, "vertex");
                break;
            case GL_GEOMETRY_SHADER:
                sprintf(strShaderType, "geometry");
                break;
            case GL_FRAGMENT_SHADER:
                sprintf(strShaderType, "fragment");
                break;
        }
        
        slog("Compile failure in %s shader:\n%s", strShaderType, strInfoLog);
        return 0;
    }
    
    return shader;
}


void RunShader()
{
	glUseProgram(ShaderPrograms[game.shader].id);
	if(ShaderPrograms[game.shader].setup != NULL)
		(*ShaderPrograms[game.shader].setup)();
}
/*eol@eof*/
