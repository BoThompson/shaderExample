#version 330 core

// Interpolated values from the vertex shaders
in vec2 UV;
in vec3 Position_worldspace;
in vec3 Normal_cameraspace;
in vec3 EyeDirection_cameraspace;
in vec3 LightDirection_cameraspace;
in vec3 Barycentric_XYZ;

// Ouput data
out vec4 color;

// Values that stay constant for the whole mesh.
uniform sampler2D myTextureSampler;
uniform mat4 MV;
uniform vec3 LightPosition_worldspace;
uniform vec3 Wireframe_color;
uniform float Wireframe_thickness;

void main()
{
	if(
		any(lessThanEqual(Barycentric_XYZ, vec3(.34 / 3.0) ) ))
	{
		color = vec4(Wireframe_color.rgb, 1.0);
	}
	else
		color = vec4(1.0, 0.0, 0.0, 0.2);
}

