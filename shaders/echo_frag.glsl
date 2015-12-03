#version 330 core

// Interpolated values from the vertex shaders
in vec2 UV;
in vec3 Position_worldspace;
in vec3 Normal_cameraspace;
in vec3 EyeDirection_cameraspace;
in vec3 LightDirection_cameraspace;
in float Echo_distance;
// Ouput data
out vec4 color;

// Values that stay constant for the whole mesh.
uniform sampler2D myTextureSampler;
uniform mat4 MV;
uniform vec3 LightPosition_worldspace;
uniform vec3 Echo_color;
uniform float Echo_radius;
uniform float Echo_band;
void main(){
	float difference = (Echo_band - abs(Echo_distance - Echo_radius)) / Echo_band;
	if(difference > 0.0)
		color = vec4(mix(Proximity_color, vec3(0.0, 0.0, 0.0), clamp(difference, 0.0, 1.0)), 1.0);
	else
		discard;

}




