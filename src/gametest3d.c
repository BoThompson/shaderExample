/**
 * gametest3d
 * @license The MIT License (MIT)
 *   @copyright Copyright (c) 2015 EngineerOfLies
 *    Permission is hereby granted, free of charge, to any person obtaining a copy
 *    of this software and associated documentation files (the "Software"), to deal
 *    in the Software without restriction, including without limitation the rights
 *    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *    copies of the Software, and to permit persons to whom the Software is
 *    furnished to do so, subject to the following conditions:
 *    The above copyright notice and this permission notice shall be included in all
 *    copies or substantial portions of the Software.
 *    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *    SOFTWARE.
 */
#include "mgl_callback.h"
#include "simple_logger.h"
#include "graphics3d.h"
#include "shader.h"
#include "obj.h"
#include "vector.h"
#include "sprite.h"
#include "entity.h"
#include "space.h"
#include "math.h"
#include <glm/gtc/matrix_transform.hpp>
#include "game.h"


#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768
#include <string.h>
#include <string>
 Vec3D cameraPosition = {0,-10,0.3f};
    Vec3D cameraRotation = {90,0,0};

GameEngine game;
float echo_center;
void set_camera(Vec3D position, Vec3D rotation);

void touch_callback(void *data, void *context)
{
    Entity *me,*other;
    Body *obody;
    if ((!data)||(!context))return;
    me = (Entity *)data;
    obody = (Body *)context;
    if (entity_is_entity(obody->touch.data))
    {
        other = (Entity *)obody->touch.data;
        //slog("%s is ",other->name);
    }
    //slog("touching me.... touching youuuuuuuu");
}

Entity *newCube(Vec3D position,const char *name)
{
    Entity * ent;
    ent = entity_new();
    if (!ent)
    {
        return NULL;
    }
    ent->objModel = obj_load("models/cube.obj");
    ent->texture = LoadSprite("models/cube_text.png",1024,1024);
    vec3d_cpy(ent->body.position,position);
    cube_set(ent->body.bounds,-1,-1,-1,2,2,2);
    sprintf(ent->name,"%s",name);
    mgl_callback_set(&ent->body.touch,touch_callback,ent);
    return ent;
}

int main(int argc, char *argv[])
{
    int i;
    float r = 0;
    Space *space;
    Entity *cube1,*cube2;
    char bGameLoopRunning = 1;
   
    SDL_Event e;
    Obj *bgobj;
    Sprite *bgtext;
    GLuint last_shader = -1;
	int shader_update = 0;
    init_logger("gametest3d.log");
    if (graphics3d_init(1024,768,1,"gametest3d",33) != 0)
    {
        return -1;
    }

    model_init();
    obj_init();
    entity_init(255);
	
    bgobj = obj_load("models/mountainvillage.obj");
    bgtext = LoadSprite("models/mountain_text.png",1024,1024);
    
    cube1 = newCube(vec3d(0,0,0),"Cubert");
	cube1->objModel->flags = OBJFLAG_SELECTED;
    cube2 = newCube(vec3d(10,0,0),"Hobbes");
    
    cube2->body.velocity.x = -0.1f;
    
    space = space_new();
    space_set_steps(space,100);
    
    space_add_body(space,&cube1->body);
    space_add_body(space,&cube2->body);
	
    while (bGameLoopRunning)
    {
        for (i = 0; i < 100;i++)
        {
            space_do_step(space);
        }
        while ( SDL_PollEvent(&e) ) 
        {
            if (e.type == SDL_QUIT)
            {
                bGameLoopRunning = 0;
            }
			else if(e.type == SDL_KEYUP)
			{
				if(e.key.keysym.sym == SDLK_LSHIFT)
				{
					REMOVE_FLAG(game.impulse_flags, IMPFLAG_SHADER_MOD);
					slog("Shader modifier mode removed\n");
				}else if(e.key.keysym.sym == SDLK_LCTRL)
				{
					REMOVE_FLAG(game.impulse_flags, IMPFLAG_GRAPHICS_MOD);
					slog("Graphics modifier mode removed\n");
				}
				
			}
            else if (e.type == SDL_KEYDOWN)
            {
                if (e.key.keysym.sym == SDLK_ESCAPE)
                {
                    bGameLoopRunning = 0;
                }else if(!IS_SET(game.impulse_flags, IMPFLAG_SHADER_MOD) && e.key.keysym.sym == SDLK_LSHIFT)
                {
					slog("Shader modifier mode initiated\n");
	                SET_FLAG(game.impulse_flags, IMPFLAG_SHADER_MOD);
                }else if(!IS_SET(game.impulse_flags, IMPFLAG_GRAPHICS_MOD) && e.key.keysym.sym == SDLK_LCTRL)
				{
					slog("Graphics modifier mode initiated\n");
	                SET_FLAG(game.impulse_flags, IMPFLAG_GRAPHICS_MOD);
				}else if(IS_SET(game.impulse_flags, IMPFLAG_GRAPHICS_MOD))
				{
					if (e.key.keysym.sym == SDLK_1)
					{
						TOGGLE_FLAG(game.graphics_flags, GFXFLAG_OUTLINE);
						slog("Outlining is %s\n", IS_SET(game.graphics_flags, GFXFLAG_OUTLINE) ? "ON" : "OFF");
						glUniform3f(ShaderPrograms[game.shader].light_position_id, cameraPosition.x, cameraPosition.y, cameraPosition.z);

					}
					else if (e.key.keysym.sym == SDLK_1)
					{
						TOGGLE_FLAG(game.graphics_flags, GFXFLAG_WIREFRAME);
						slog("Wireframe is %s\n", IS_SET(game.graphics_flags, GFXFLAG_WIREFRAME) ? "ON" : "OFF");
						glUniform3f(ShaderPrograms[game.shader].light_position_id, cameraPosition.x, cameraPosition.y, cameraPosition.z);

					}
				}else if(IS_SET(game.impulse_flags, IMPFLAG_SHADER_MOD))
				{
					//The impulse for shader modifications is set, shader controls
					if (e.key.keysym.sym == SDLK_1)
					{
						game.shader = SHADERPROG_BASE;
					
						if(last_shader != game.shader){
							slog("Base Shader activated\n");
							last_shader = game.shader;
							RunShader();
						}
						glUniform3f(ShaderPrograms[game.shader].light_position_id, cameraPosition.x, cameraPosition.y, cameraPosition.z);

					}
					else if (e.key.keysym.sym == SDLK_2)
					{
						game.shader = SHADERPROG_PROXIMITY;
						if(last_shader != game.shader){
							slog("Proximity Shader activated\n");
							last_shader = game.shader;
							RunShader();
						}
					}
					else if(e.key.keysym.sym == SDLK_3)
					{
						game.shader = SHADERPROG_WIREFRAME;
						if(last_shader != game.shader){
							slog("Wireframe Shader activated\n");
							last_shader = game.shader;
							RunShader();
						}
					
					}
					else if(e.key.keysym.sym == SDLK_4)
					{
						game.shader = SHADERPROG_DETECTIVE;
						if(last_shader != game.shader){
							slog("Detective Shader activated\n");
							last_shader = game.shader;
							RunShader();
						}
					
					}
					else if(e.key.keysym.sym == SDLK_5)
					{
						game.shader = SHADERPROG_ECHO;
						if(last_shader != game.shader){
							slog("Echo Shader activated\n");
							last_shader = game.shader;
							RunShader();
						}
					
					}
					else if(e.key.keysym.sym == SDLK_MINUS)
					{
						if(--ShaderPrograms[game.shader].complexity < 0)
							ShaderPrograms[game.shader].complexity = 0;
					}
					else if(e.key.keysym.sym == SDLK_PLUS)
					{
						if(++ShaderPrograms[game.shader].complexity > ShaderPrograms[game.shader].max_complexity)
							ShaderPrograms[game.shader].complexity = ShaderPrograms[game.shader].max_complexity;
					}else if(e.key.keysym.sym == SDLK_KP_7)
					{
						switch(game.shader)
						{
						case SHADERPROG_PROXIMITY:
							//Make it more red
							ShaderPrograms[game.shader].values[0]+= 0.01f;
							if(ShaderPrograms[game.shader].values[0] > 1.0f)
								ShaderPrograms[game.shader].values[0] = 1.0f;
							shader_update = 1;
							break;
						default: break;
						}
					}else if(e.key.keysym.sym == SDLK_KP_4)
					{
						switch(game.shader)
						{
						case SHADERPROG_PROXIMITY:
							//Make it less red
							ShaderPrograms[game.shader].values[0]-= 0.01f;
							if(ShaderPrograms[game.shader].values[0] < 0)
								ShaderPrograms[game.shader].values[0] = 0;
							shader_update = 1;
							break;
						default: break;
						}
					}else if(e.key.keysym.sym == SDLK_KP_8)
					{
						switch(game.shader)
						{
						case SHADERPROG_PROXIMITY:
							//Make it more green
							ShaderPrograms[game.shader].values[1]+= 0.01f;
							if(ShaderPrograms[game.shader].values[1] > 1.0f)
								ShaderPrograms[game.shader].values[1] = 1.0f;
							shader_update = 1;
							break;
						default: break;
						}
					}else if(e.key.keysym.sym == SDLK_KP_5)
					{
						switch(game.shader)
						{
						case SHADERPROG_PROXIMITY:
							//Make it less green
							ShaderPrograms[game.shader].values[1]-= 0.01f;
							if(ShaderPrograms[game.shader].values[1] < 0)
								ShaderPrograms[game.shader].values[1] = 0;
							shader_update = 1;
							break;
						default: break;
						}
					}
					else if(e.key.keysym.sym == SDLK_KP_9)
					{
						switch(game.shader)
						{
						case SHADERPROG_PROXIMITY:
							//Make it more blue
							ShaderPrograms[game.shader].values[2]+= 0.01f;
							if(ShaderPrograms[game.shader].values[2] > 1.0f)
								ShaderPrograms[game.shader].values[2] = 1.0f;
							shader_update = 1;
							break;
						default: break;
						}
					}else if(e.key.keysym.sym == SDLK_KP_6)
					{
						switch(game.shader)
						{
						case SHADERPROG_PROXIMITY:
							//Make it less blue
							ShaderPrograms[game.shader].values[2]-= 0.01f;
							if(ShaderPrograms[game.shader].values[2] < 0)
								ShaderPrograms[game.shader].values[2] = 0;
							shader_update = 1;
							break;
						default: break;
						}
					}else if(e.key.keysym.sym == SDLK_KP_PLUS)
					{
						switch(game.shader)
						{
						case SHADERPROG_PROXIMITY:
							//Make it longer range
							ShaderPrograms[game.shader].values[3]+= 1.0f;
							if(ShaderPrograms[game.shader].values[3] > 1000.0f)
								ShaderPrograms[game.shader].values[3] = 1000.0f;
							shader_update = 1;
							break;
						case SHADERPROG_WIREFRAME:
							//Make it thicker
							ShaderPrograms[game.shader].values[3]+= 0.01f;
							if(ShaderPrograms[game.shader].values[3] > 1.0f)
								ShaderPrograms[game.shader].values[3] = 1.0f;
							shader_update = 1;
							break;
						case SHADERPROG_ECHO:
							//Make it wider
							ShaderPrograms[game.shader].values[3]+= 1.0f;
							if(ShaderPrograms[game.shader].values[3] > 100.0f)
								ShaderPrograms[game.shader].values[3] = 100.0f;
							shader_update = 1;
							break;
						default: break;
						}
					}else if(e.key.keysym.sym == SDLK_KP_MINUS)
					{
						switch(game.shader)
						{
						case SHADERPROG_PROXIMITY:
							//Make it shorter range
							ShaderPrograms[game.shader].values[3]-= 1.0f;
							if(ShaderPrograms[game.shader].values[3] < 1.0f)
								ShaderPrograms[game.shader].values[3] = 1.0f;
							shader_update = 1;
							break;
						case SHADERPROG_WIREFRAME:
							//Make it thinner
							ShaderPrograms[game.shader].values[3]-= 0.01f;
							if(ShaderPrograms[game.shader].values[3] < 0.0f)
								ShaderPrograms[game.shader].values[3] = 0.0f;
							shader_update = 1;
							break;
						case SHADERPROG_ECHO:
							//Make it narrower
							ShaderPrograms[game.shader].values[3]-= 1.0f;
							if(ShaderPrograms[game.shader].values[3] < 1.0f)
								ShaderPrograms[game.shader].values[3] = 1.0f;
							shader_update = 1;
							break;
						default: break;
						}
					}
				}else{
					//No modifier impulse is set, regular controls
					if (e.key.keysym.sym == SDLK_SPACE)
					{
						cameraPosition.z++;
					}
					else if (e.key.keysym.sym == SDLK_z)
					{
						cameraPosition.z--;
					}
					else if (e.key.keysym.sym == SDLK_w)
					{
						vec3d_add(
							cameraPosition,
							cameraPosition,
							vec3d(
								-sinf(cameraRotation.z * DEGTORAD),
								cosf(cameraRotation.z * DEGTORAD),
								0.0f
							));
					}
					else if (e.key.keysym.sym == SDLK_s)
					{
						vec3d_add(
							cameraPosition,
							cameraPosition,
							vec3d(
								sinf(cameraRotation.z * DEGTORAD),
								-cosf(cameraRotation.z * DEGTORAD),
								0.0f
							));
					}
					else if (e.key.keysym.sym == SDLK_d)
					{
						vec3d_add(
							cameraPosition,
							cameraPosition,
							vec3d(
								cosf(cameraRotation.z * DEGTORAD),
								sinf(cameraRotation.z * DEGTORAD),
								0
							));
					}
					else if (e.key.keysym.sym == SDLK_a)
					{
						vec3d_add(
							cameraPosition,
							cameraPosition,
							vec3d(
								-cosf(cameraRotation.z * DEGTORAD),
								-sinf(cameraRotation.z * DEGTORAD),
								0
							));
					}
					else if (e.key.keysym.sym == SDLK_LEFT)
					{
						cameraRotation.z += 1;
					}
					else if (e.key.keysym.sym == SDLK_RIGHT)
					{
						cameraRotation.z -= 1;
					}
					else if (e.key.keysym.sym == SDLK_UP)
					{
						cameraRotation.x += 1;
					}
					else if (e.key.keysym.sym == SDLK_DOWN)
					{
						cameraRotation.x -= 1;
					}
				}
            }
        }
		if(last_shader != game.shader){
			last_shader = game.shader;
			RunShader();
		}else if(shader_update)
		{
			shader_update = 0;
			if(ShaderPrograms[game.shader].setup != NULL)
				(*ShaderPrograms[game.shader].setup)();
		}
        graphics3d_frame_begin();

        set_camera(
            cameraPosition,
            cameraRotation);
        
        entity_draw_all();  
        obj_draw(
            bgobj,
            vec3d(0,0,2),
            vec3d(90,90,0),
            vec3d(5,5,5),
            vec4d(1,1,1,1),
            bgtext,
			0
        );
        
        if (r > 360)r -= 360;
        /* drawing code above here! */
        graphics3d_next_frame();
    } 
    return 0;
}

void set_camera(Vec3D position, Vec3D rotation)
{
	glm::mat4 Rotation = glm::rotate(glm::mat4(), glm::radians(-rotation.z), glm::vec3(0.0f, 0.0f, 1.0f))
				  * glm::rotate(glm::mat4(), glm::radians(-rotation.y), glm::vec3(0.0f, 1.0f, 0.0f))
				  * glm::rotate(glm::mat4(), glm::radians(-rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    glm::mat4 Translation = glm::translate(glm::mat4(), glm::vec3(-position.x,-position.y,-position.z));
	game.View = Translation * Rotation;
}

/*eol@eof*/