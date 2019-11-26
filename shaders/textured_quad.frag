
#version 330 core

in vec2 texcoord;

uniform sampler2D texture0;

out vec4 color;

void main() {
    // vec2 testTexcoord = vec2(0.5, 0.5);
    // vec3 textureContrib = texture(texture0, testTexcoord).rgb;
    
    vec4 textureContrib = texture(texture0, texcoord).rgba;
    color.rgba = textureContrib;
}
