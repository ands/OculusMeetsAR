#version 130

uniform sampler2D coordinateMap;
uniform sampler2D videoTexture;

in vec2 oUV;

out vec4 fragColor;

void main(void)
{
	vec2 uv = texture(coordinateMap, vec2(/*1.0 - */oUV.x, oUV.y)).ba;
	fragColor = texture(videoTexture, vec2(/*1.0 - */uv.x, uv.y));
}
