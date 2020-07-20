#version 330 core

in vec2 texcoord;
in vec4 fs_color;

uniform sampler2D fontTable;

out vec4 color;

void main() {
    vec4 textureContrib = texture(fontTable, texcoord).rgba;
    
    color.rgba = textureContrib;
    color.rgb = fs_color.rgb;

    //color.rgba = vec4(1, 0, 0, 1);
}
