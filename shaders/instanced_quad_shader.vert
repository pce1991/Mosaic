#version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec4 instance_color;
layout(location = 2) in mat4 instance_model;

uniform mat4 viewProjection;

out vec4 fs_color;

void main() {
    mat4 model = mat4(1);
    gl_Position = viewProjection * instance_model * vec4(vertexPosition_modelspace, 1.0f);
    //gl_Position = viewProjection * model * vec4(vertexPosition_modelspace, 1.0f);

    fs_color = instance_color;
}

