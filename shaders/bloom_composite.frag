#version 330 core

in vec2 texcoord;

uniform sampler2D baseTexture;
uniform sampler2D meltTexture;
uniform sampler2D narrowTexture;
uniform sampler2D wideTexture;
uniform float softness;
uniform float wideMix;
uniform float strength;

out vec4 fragColor;

void main() {
    vec4 base = texture(baseTexture, texcoord);
    vec4 melt = texture(meltTexture, texcoord);
    vec3 narrow = texture(narrowTexture, texcoord).rgb;
    vec3 wide = texture(wideTexture, texcoord).rgb;

    // Crossfade the sharp layer toward the blurred one. Mixing premultiplied
    // vec4 keeps alpha edges soft too, and leaves glow energy untouched.
    vec4 layer = mix(base, melt, softness);

    // Both bands are normalized, so this weighted sum keeps the glow energy
    // equal to one blur while the wide band carries part of it further out.
    vec3 glow = mix(narrow, wide, wideMix);

    fragColor = vec4(layer.rgb + glow * strength, layer.a);
}
