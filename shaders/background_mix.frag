#version 330 core

in vec2 texcoord;

uniform sampler2D sourceTexture;
uniform sampler2D backgroundTexture;
uniform sampler2D noiseTexture;
uniform float mixStrength;
uniform float noiseScale;
uniform vec2 noiseOffset;
uniform float noiseThreshold;
uniform float noiseEdge;

out vec4 fragColor;

void main() {
    vec4 src = texture(sourceTexture, texcoord);
    vec4 bg = texture(backgroundTexture, texcoord);
    float noise = texture(noiseTexture, texcoord * noiseScale + noiseOffset).r;

    // Remap the noise through a smoothstep so the background appears in large,
    // feathered regions instead of a soft gradient. noiseEdge controls how soft
    // the boundary between regions is.
    float mask = smoothstep(noiseThreshold - noiseEdge, noiseThreshold + noiseEdge, noise);

    // Additive double exposure: both layers stay visible, and mixStrength scales how
    // much of the (premultiplied) background is added on top of the tiles.
    float w = clamp(mixStrength * mask, 0.0, 1.0);
    fragColor = vec4(src.rgb + bg.rgb * w, src.a);
}
