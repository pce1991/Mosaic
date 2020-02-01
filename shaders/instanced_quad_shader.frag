#version 330 core

in vec4 fs_color;

out vec4 color_;

void main(){
    color_ = fs_color;
    //color_ = vec4(1, 1, 1, 1);
}

