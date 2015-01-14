#version 130

uniform sampler2D diffuseMap;

void main(void)
{
	gl_FragColor = texture2D(diffuseMap, gl_TexCoord[0].xy);
}
