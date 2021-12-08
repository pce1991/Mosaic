#version 330 core

in vec4 fs_color;
in vec3 fs_normal;

out vec4 color_;

void main(void){
    color_ = fs_color;
}

