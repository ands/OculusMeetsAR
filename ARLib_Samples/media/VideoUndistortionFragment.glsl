#version 130

uniform sampler2D coordinateMap;
uniform sampler2D videoTexture;

in vec2 oUV;

out vec4 fragColor;

void main(void)
{
	vec2 uv = texture(coordinateMap, oUV).ba;
	fragColor = texture(videoTexture, uv);
}
