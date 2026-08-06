#version 330 core

in vec2 texcoord;
in vec4 fs_color;

uniform sampler2D texture0;

out vec4 color_;

void main() {
    color_ = texture(texture0, texcoord) * fs_color;
}
