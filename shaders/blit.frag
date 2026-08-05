#version 330 core

in vec2 texcoord;
uniform sampler2D uiTexture;

out vec4 fragColor;

void main() {
    fragColor = texture(uiTexture, texcoord);
}
