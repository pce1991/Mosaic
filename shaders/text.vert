#version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 in_uv;
layout(location = 2) in int in_codepoint;
layout(location = 3) in vec4 in_color;
layout(location = 4) in vec2 in_dimensions;
layout(location = 5) in vec3 in_position;

uniform mat4 model;
uniform mat4 viewProjection;

uniform sampler1D texcoordsMap;
uniform float time;

out vec2 texcoord;
out vec4 fs_color;

void main() {
    vec3 modelPos = in_position;

    modelPos.x += vertexPosition_modelspace.x * in_dimensions.x;
    modelPos.y += vertexPosition_modelspace.y * in_dimensions.y;
    
    gl_Position = viewProjection * model * vec4(modelPos, 1.0f);

    vec4 texcoords = texelFetch(texcoordsMap, in_codepoint, 0);
    vec2 texcoord0 = texcoords.xy;
    vec2 texcoord1 = texcoords.zw;
    vec2 uv = in_uv;
    // @GACK: this shouldnt be necessary, must be baking something incorrectly.
    uv.y = 1 - uv.y;
    texcoord = mix(texcoord0, texcoord1, uv);

    fs_color = in_color;
}

