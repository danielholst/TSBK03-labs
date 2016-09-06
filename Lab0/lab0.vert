/*
 * random comment here
 * makes syntax highlight appaer
 * colors like springs sprouts
 */

#version 150

in  vec3  in_Position;
in vec3 in_Position2;
in  vec3  in_Normal;
in vec3 in_Normal2;
in  vec2  in_TexCoord;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;

out float shade;
out vec3 vertexNormal;
void main(void)
{
	vertexNormal = normalize((mat3(viewMatrix)*in_Normal)); // Fake shading

	gl_Position=projectionMatrix*viewMatrix*vec4(in_Position, 1.0);
    
}

