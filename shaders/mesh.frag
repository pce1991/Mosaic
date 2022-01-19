#version 330 core

in vec4 fs_color;
out vec4 fragColor;
in vec3 fs_normal;

//out vec4 color_;

void main(void){
    fragColor = fs_color;
}

