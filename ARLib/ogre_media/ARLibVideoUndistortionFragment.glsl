#version 130

uniform sampler2D coordinateMap;
uniform sampler2D videoTexture;

in vec2 oUV;

out vec4 fragColor;

void main(void)
{
	// TODO: do the axis inversion in the coordinate map!
	vec2 uv = texture(coordinateMap, oUV).ba;
	fragColor = texture(videoTexture, uv);
}
