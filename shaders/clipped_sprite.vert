#version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 in_texcoord;

uniform mat4 model;
uniform mat4 viewProjection;



out vec2 texcoord;

void main() {
    gl_Position = viewProjection * model * vec4(vertexPosition_modelspace, 1.0f);

    texcoord = in_texcoord;
}

