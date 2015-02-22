#version 130

uniform mat4 worldViewProj;

in vec4 vertex;
in vec2 uv0;
in vec2 uv1;

out vec2 oUV0;
out vec2 oUV1;

void main(void)
{
	gl_Position = worldViewProj * vertex;
    oUV0 = uv0;
	oUV1 = uv1;
}
