#version 330 core

in vec4 fs_color;
in vec2 fs_texcoords;

out vec4 fragColor;

void main(void){
    fragColor = fs_color;

    fragColor.rgb = vec3(0);
    fragColor.r = (1 + sin(fs_texcoords.x * (60 * fs_texcoords.y))) * 0.5;
    fragColor.b = (1 + sin(fs_texcoords.y * (20 * fs_texcoords.x))) * 0.5;
    fragColor.g = fs_texcoords.y;
}
