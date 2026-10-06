#version 330 core

in vec2 texcoord;
uniform sampler2D sourceTexture;
uniform float threshold;

out vec4 fragColor;

void main() {
    vec4 color = texture(sourceTexture, texcoord);
    float brightness = max(color.r, max(color.g, color.b));
    float mask = smoothstep(threshold, threshold + 0.05, brightness);
    fragColor = vec4(color.rgb * mask, 1.0);
}
