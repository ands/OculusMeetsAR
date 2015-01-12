#version 130

// Values automatically defined by Ogre/OpenGL:
in vec4 vertex;
in vec2 uv0;

// Load in values defined in the material:
uniform mat4 worldViewProj;
//uniform vec2 eyeToSourceUVScale;
//uniform vec2 eyeToSourceUVOffset;
//uniform mat4 eyeRotationStart;
//uniform mat4 eyeRotationEnd;

void main(void)
{
	gl_Position = worldViewProj * vertex;
    gl_TexCoord[0].xy = uv0;
}
