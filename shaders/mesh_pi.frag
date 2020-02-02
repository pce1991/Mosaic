#version 120

attribute vec4 fs_color;

varying vec4 color_;

void main(void){
    color_ = fs_color;
}

