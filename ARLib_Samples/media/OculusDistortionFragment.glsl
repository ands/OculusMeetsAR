#version 130

uniform sampler2D diffuseMap;

in vec4 oColor;

out vec4 fragColor;

void main()
{
	float r = texture2D(diffuseMap, gl_TexCoord[0].xy).r;
	float g = texture2D(diffuseMap, gl_TexCoord[1].xy).g;
	float b = texture2D(diffuseMap, gl_TexCoord[2].xy).b;
	fragColor = vec4(r, g, b, 1.0) * oColor;
}
