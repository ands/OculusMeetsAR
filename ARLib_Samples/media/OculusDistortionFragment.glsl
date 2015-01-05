uniform sampler2D diffuseMap;

varying vec4 oColor;

void main(void)
{
	float r = texture2D(diffuseMap, gl_TexCoord[0].xy).r;
	float g = texture2D(diffuseMap, gl_TexCoord[1].xy).g;
	float b = texture2D(diffuseMap, gl_TexCoord[2].xy).b;
	gl_FragColor = vec4(r, g, b, 1.0) * oColor;
}
