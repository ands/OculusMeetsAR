#version 130

uniform mat4 worldViewProj;

in vec4 vertex;
in vec2 uv0;

out vec2 oUV;

void main(void)
{
	gl_Position = worldViewProj * vertex;
	oUV = uv0.yx;
}
