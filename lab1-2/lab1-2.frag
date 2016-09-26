#version 150
// bump mapping should be calculated
// 1) in view coordinates
// 2) in texture coordinates

in vec2 outTexCoord;
in vec3 out_Normal;
in vec3 Ps;
in vec3 Pt;
in vec3 pixPos;  // Needed for specular reflections
uniform sampler2D texUnit;
out vec4 out_Color;

void main(void) {
    vec3 light = vec3(0.0, 0.7, 0.7); // Light source in view coordinates
    // Calculate gradients here
    float offset = 1.0 / 256.0; // texture size, same in both directions
    
    vec2 tc = outTexCoord;
    // Calculate ds and dt
    // View coordinates
    vec4 ds = texture(texUnit, vec2(tc.x+offset, tc.y)) - texture(texUnit, vec2(tc.x-offset, tc.y));
    vec4 dt = texture(texUnit, vec2(tc.x, tc.y+offset)) - texture(texUnit, vec2(tc.x, tc.y-offset));
    
    // View coordinates
    vec3 unitNormal = normalize(out_Normal);
    unitNormal = unitNormal + vec3(ds)*Ps + vec3(dt)*Pt;
    
    // Simplified lighting calculation.
    // A full solution would include material, ambient, specular, light sources, multiply by texture.
    out_Color = vec4( dot(unitNormal, light)) * texture(texUnit, outTexCoord);
    
    // Mvt matrix - texture coordinates
    /*mat3 mvt = transpose(mat3(Ps, Pt, out_Normal));
     light = mvt * light;
     vec3 unitNormal = normalize(vec3(ds.x, dt.x, 1.0));
     out_Color = vec4( dot(unitNormal, light)) * texture(texUnit, outTexCoord);*/
}