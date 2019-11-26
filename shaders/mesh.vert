#version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace;

uniform mat4 model;
uniform mat4 viewProjection;
uniform vec4 color;

out vec4 fs_color;

void main() {
    gl_Position = viewProjection * model * vec4(vertexPosition_modelspace, 1.0f);

    fs_color = color;
}

