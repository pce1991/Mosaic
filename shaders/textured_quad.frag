
#version 330 core

in vec2 texcoord;

uniform sampler2D texture0;
uniform float time;

out vec4 color;

void main() {
    // vec2 testTexcoord = vec2(0.5, 0.5);
    // vec3 textureContrib = texture(texture0, testTexcoord).rgb;

    vec2 uv = texcoord;
    //uv.x += ((1 + sin(time)) * 0.5);

    // uv *= ((1 + sin(time)) * 0.5);
    // uv += vec2(0.5, 0.5);

    // uv.x = (1 + cos(uv.y * 3.14)) * 0.5;
    // uv.y = (1 + sin(uv.x * 3.14)) * 0.5;
    
    vec2 uvCenter = vec2(0.5, 0.5);
    //vec2 uvCenter = vec2(0.5, 0.5) + (0.1 * vec2(cos(time), sin(time)));
    vec2 uvFromCenter = uv - uvCenter;

    

    // uv.x += ((1 + sin((time + (uv.y * 8)))) * 0.5);
    // uv.y += time * 0.1;
    // uv.y += time * 0.2 + ((1 + cos((time + (uv.x * 2)))) * 0.5);

    // uv.x += cos(time);
    // uv.y += sin(time);

    // uv = uvFromCenter;
    // uv.x += (1 + cos(uvFromCenter.x * uvFromCenter.y * 8 + time)) * 0.5;
    // uv.y += (1 + sin(uvFromCenter.x * uvFromCenter.y * 8 + time)) * 0.5;

    //uv += vec2(0.5, -0.5);

    vec2 uvDir = normalize(uvFromCenter);
    float d = dot(uvDir, vec2(0, -1));

    float angle = acos(d) * 0.5;

    // uv = vec2(cos(angle) * (uv.x - 0.5) - sin(angle) * (uv.y - 0.5) + 0.5,
    //           cos(angle) * (uv.y - 0.5) + sin(angle) * (uv.x - 0.5) + 0.5);

    //uv += d;
    //uv *= d;

    // uv.x *= atan(uv.x, uv.y) * time;
    // uv.y *= atan(uv.x, uv.y) * time;
    
    vec4 textureContrib = texture(texture0, uv).rgba;
    
    color.rgba = textureContrib;

    //color.rgb = vec3((1 + d) / 2);

    // color.rgb = vec3(0, 0, 0);
    // color.rgb = vec3(uv.x, uv.y, 0);

    //color.rgba = vec4(texcoord.x, texcoord.y, 0, 1);
}
