
#version 330 core

in vec2 texcoord;

uniform sampler2D texture0;

// min and max
uniform vec4 pixelBorder;

out vec4 color;

void main() {
    // vec2 testTexcoord = vec2(0.5, 0.5);
    // vec3 textureContrib = texture(texture0, testTexcoord).rgb;

    if (gl_FragCoord.x < pixelBorder.x ||
        gl_FragCoord.y < pixelBorder.y ||
        gl_FragCoord.x > pixelBorder.z ||
        gl_FragCoord.y > pixelBorder.w) {
        discard;
    }
    
    vec4 textureContrib = texture(texture0, texcoord).rgba;
    color.rgba = textureContrib;
}
