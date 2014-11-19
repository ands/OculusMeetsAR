#version 330 core

layout(location = 0) in vec3 vertexPosition;
uniform mat4 Transform;

void main(){
	gl_Position.xyz = (Transform* vec4(vertexPosition,1.0)).xyz;
	gl_Position.w = 1.0;
}