#version 130

uniform vec2 offset;
uniform vec2 scale;

in vec4 vertex;
in vec2 uv0;

out vec2 oUV;

void main(void)
{
	gl_Position = vertex;
	oUV = (uv0 + offset) * scale + vec2(0.5f);
}
