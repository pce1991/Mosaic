#version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 in_texcoord;
layout(location = 2) in vec4 instance_color;
layout(location = 3) in mat4 instance_model;

uniform mat4 viewProjection;

out vec2 texcoord;
out vec4 fs_color;

void main() {
    gl_Position = viewProjection * instance_model * vec4(vertexPosition_modelspace, 1.0f);

    texcoord = in_texcoord;
    fs_color = instance_color;
}
