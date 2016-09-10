#version 150

in vec2 outTexCoord;
uniform sampler2D texUnit;
out vec4 color;

void main() {
    
    float offset = 1.0 / 512;
    vec2 tc = outTexCoord;
    
    vec4 c = texture(texUnit, tc);
    tc.x = tc.x + offset;
    vec4 l = texture(texUnit, tc);
    tc.x = tc.x - 2.0*offset;
    vec4 r = texture(texUnit, tc);
    tc.x = tc.x - offset;
    color = (c + c + l + r) * 0.25;
}