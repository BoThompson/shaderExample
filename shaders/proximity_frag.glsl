#version 330 core

// Interpolated values from the vertex shaders
in vec2 UV;
in vec3 Position_worldspace;
in vec3 Normal_cameraspace;
in vec3 EyeDirection_cameraspace;
in vec3 LightDirection_cameraspace;

// Ouput data
out vec4 color;

// Values that stay constant for the whole mesh.
uniform sampler2D myTextureSampler;
uniform mat4 MV;
uniform vec3 LightPosition_worldspace;
uniform vec3 Proximity_color;
uniform float Proximity_unit;

void main(){

	color = vec4(mix(Proximity_color, vec3(0.0, 0.0, 0.0), length(EyeDirection_cameraspace) / Proximity_unit), 1.0);

}




