/*
 * random comment here
 * makes syntax highlight appaer
 * colors like springs sprouts
 */

#version 150

in float shade;
in vec3 vertexNormal;
in vec2 texCoord;

uniform sampler2D exampleTexture;
out vec4 out_Color;

const vec3 lightDir = normalize(vec3(0.5, 0.5, 0.5));
const vec3 lightColor = vec3(1.0,1.0,1.0);

void main(void)
{
    float cosTheta = clamp(dot(lightDir, vertexNormal), 0, 1);
	out_Color = texture(exampletexture, texCoord)cosTheta * vec4(lightColor, 1.0);
}

