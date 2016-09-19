#version 150

in vec2 outTexCoord;
uniform sampler2D texUnit;
out vec4 color;

void main(void)
{
    vec4 col = texture(texUnit, outTexCoord);
    color.r = max(col.r - 1.0, 0.0);
    color.g = max(col.g - 1.0, 0.0);
    color.b = max(col.b - 1.0, 0.0);
}
