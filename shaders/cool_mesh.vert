#version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 in_texcoords;


uniform mat4 model;
uniform mat4 viewProjection;
uniform vec4 color;
uniform float time;

out vec4 fs_color;
out vec2 fs_texcoords;

void main() {
    // vec3 vertex = vertexPosition_modelspace;

    // if (vertex.x > 0 && vertex.y > 0) {
    //     vertex.x += sin(time);
    // }
    // if (vertex.x > 0 && vertex.y < 0) {
    //     vertex.x += cos(time);
    // }
    // if (vertex.x < 0 && vertex.y > 0) {
    //     vertex.y += sin(time + 1.5);
    // }
    // if (vertex.x < 0 && vertex.y < 0) {
    //     vertex.y += cos(time + 1.5);
    // }
    
    // gl_Position = viewProjection * model * vec4(vertex, 1.0f);

    // fs_color = color;

    // fs_color.r = (1 + sin(vertex.x * 3)) * 0.5;
    // fs_color.g = (1 + cos(vertex.y * 2)) * 0.5;

    // // fs_color.r = (1 + sin(time)) * 0.5;
    // // fs_color.g = (1 + sin(time * 2 + 3.14)) * 0.5;
    // fs_color.b = (1 + sin(vertex.x + vertex.y)) * 0.5f;

    gl_Position = viewProjection * model * vec4(vertexPosition_modelspace, 1.0f);
    fs_color = color;

    fs_color.rgb = vec3(0);

    // @NOTE: this wont work because our in_texcoord is going to be a fixed value
    // in the vertex shader, but we want to use the interpolated value.
    // The reason using it for colors works is because the colors are interpolated
    // since they're sent to the fragment shader.
    //fs_color.r = (1 + sin((in_texcoords.x * 12.4))) * 0.5;

    // Visualize texcoords
    fs_color.rgb = vec3(0);
    fs_color.r = in_texcoords.x;
    fs_color.b = in_texcoords.y;

    fs_texcoords = in_texcoords;
}

