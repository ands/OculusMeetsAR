#version 130

uniform sampler2D Blur0;
uniform vec2 BlurRadius;

in vec2 oUV;

out vec4 fragColor;

vec2 pos[11] = vec2[11]
(
	vec2(0, -5), vec2(0, -4), vec2(0, -3), vec2(0, -2), vec2(0, -1),
	vec2(0, 0),
	vec2(0, 1), vec2(0, 2), vec2(0, 3), vec2(0, 4), vec2(0, 5)
);

float samples[11] = float[11]
(
	0.01222447, 0.02783468, 0.06559061, 0.12097757, 0.17466632,
	0.19741265,
	0.17466632, 0.12097757, 0.06559061, 0.02783468, 0.01222447
);

void main()
{
    vec4 sum = vec4(0);
    for(int i = 0; i < 11; i++)
        sum += texture(Blur0, oUV + pos[i] * BlurRadius) * samples[i];
    fragColor = sum;
}
