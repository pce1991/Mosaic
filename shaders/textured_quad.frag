
#version 330 core

in vec2 texcoord;

uniform sampler2D texture0;

out vec4 color;

void main() {
    vec2 uv = texcoord;

    vec2 uvCenter = vec2(0.5, 0.5);
    
    vec2 uvFromCenter = uv - uvCenter;

    vec2 uvDir = normalize(uvFromCenter);
    float d = dot(uvDir, vec2(0, -1));

    float angle = acos(d) * 0.5;
    
    vec4 textureContrib = texture(texture0, uv).rgba;
    
    color.rgba = textureContrib;
}
