#version 330 core

in vec2 texcoord;
uniform sampler2D sourceTexture;
uniform vec2 direction;

out vec4 fragColor;

void main() {
    vec4 sum = texture(sourceTexture, texcoord) * 0.20417;
    sum += texture(sourceTexture, texcoord + direction * 1.0) * 0.18018;
    sum += texture(sourceTexture, texcoord - direction * 1.0) * 0.18018;
    sum += texture(sourceTexture, texcoord + direction * 2.0) * 0.12383;
    sum += texture(sourceTexture, texcoord - direction * 2.0) * 0.12383;
    sum += texture(sourceTexture, texcoord + direction * 3.0) * 0.06629;
    sum += texture(sourceTexture, texcoord - direction * 3.0) * 0.06629;
    sum += texture(sourceTexture, texcoord + direction * 4.0) * 0.02762;
    sum += texture(sourceTexture, texcoord - direction * 4.0) * 0.02762;
    fragColor = sum;
}
