/************************************************************************************
 
 Source      :   CAPI_GL_Shaders.h
 Content     :   Distortion shader header for GL
 Created     :   November 11, 2013
 Authors     :   David Borel, Volga Aksoy
 Modified by :   Andreas Mantler
 
 Copyright   :   Copyright 2014 Oculus VR, LLC All Rights reserved.

Licensed under the Oculus VR Rift SDK License Version 3.2 (the "License"); 
you may not use the Oculus VR Rift SDK except in compliance with the License, 
which is provided at the time of installation or download, or which 
otherwise accompanies this software in either electronic or hard copy form.

You may obtain a copy of the License at

http://www.oculusvr.com/licenses/LICENSE-3.2 

Unless required by applicable law or agreed to in writing, the Oculus VR SDK 
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
 
 ************************************************************************************/

#version 130

// Values automatically defined by Ogre/OpenGL:
in vec4 vertex;
in vec2 uv0;			// Red channel
in vec2 uv1;			// Green channel
in vec2 uv2;			// Blue channel
in vec4 colour;		// Vertex Colour

// Load in values defined in the material:
uniform mat4 worldViewProj;
uniform vec2 eyeToSourceUVScale;
uniform vec2 eyeToSourceUVOffset;
//uniform mat4 eyeRotationStart;
//uniform mat4 eyeRotationEnd;

out vec4 oColor;

void main()
{
	gl_Position = worldViewProj * vertex;
	
    // Vertex inputs are in TanEyeAngle space for the R,G,B channels (i.e. after chromatic aberration and distortion).
    // These are now "real world" vectors in direction (x,y,1) relative to the eye of the HMD.
/*	vec3 TanEyeAngleR = vec3(uv0, 1.0);
    vec3 TanEyeAngleG = vec3(uv1, 1.0);
    vec3 TanEyeAngleB = vec3(uv2, 1.0);
	
    // Accurate time warp lerp vs. faster
    // Apply the two 3x3 timewarp rotations to these vectors.
	vec3 TransformedRStart = (eyeRotationStart * vec4(TanEyeAngleR, 0)).xyz;
	vec3 TransformedGStart = (eyeRotationStart * vec4(TanEyeAngleG, 0)).xyz;
	vec3 TransformedBStart = (eyeRotationStart * vec4(TanEyeAngleB, 0)).xyz;
	vec3 TransformedREnd   = (eyeRotationEnd * vec4(TanEyeAngleR, 0)).xyz;
	vec3 TransformedGEnd   = (eyeRotationEnd * vec4(TanEyeAngleG, 0)).xyz;
	vec3 TransformedBEnd   = (eyeRotationEnd * vec4(TanEyeAngleB, 0)).xyz;
	
    // And blend between them.
    vec3 TransformedR = mix(TransformedRStart, TransformedREnd, colour.a);
    vec3 TransformedG = mix(TransformedGStart, TransformedGEnd, colour.a);
    vec3 TransformedB = mix(TransformedBStart, TransformedBEnd, colour.a);
    
    // Project them back onto the Z=1 plane of the rendered images.
    float RecipZR = 1.0 / TransformedR.z;
    float RecipZG = 1.0 / TransformedG.z;
    float RecipZB = 1.0 / TransformedB.z;
    vec2 FlattenedR = vec2(TransformedR.x * RecipZR, TransformedR.y * RecipZR);
    vec2 FlattenedG = vec2(TransformedG.x * RecipZG, TransformedG.y * RecipZG);
    vec2 FlattenedB = vec2(TransformedB.x * RecipZB, TransformedB.y * RecipZB);
    
    // These are now still in TanEyeAngle space.
    // Scale them into the correct [0-1],[0-1] UV lookup space (depending on eye)
    vec2 SrcCoordR = FlattenedR * eyeToSourceUVScale + eyeToSourceUVOffset;
    vec2 SrcCoordG = FlattenedG * eyeToSourceUVScale + eyeToSourceUVOffset;
    vec2 SrcCoordB = FlattenedB * eyeToSourceUVScale + eyeToSourceUVOffset; */
    
	// Without timewarp
	vec2 SrcCoordR = uv0 * eyeToSourceUVScale + eyeToSourceUVOffset;
    vec2 SrcCoordG = uv1 * eyeToSourceUVScale + eyeToSourceUVOffset;
    vec2 SrcCoordB = uv2 * eyeToSourceUVScale + eyeToSourceUVOffset;
	
    gl_TexCoord[0] = vec4(SrcCoordR, 0.0, 0.0);
    gl_TexCoord[1] = vec4(SrcCoordG, 0.0, 0.0);
    gl_TexCoord[2] = vec4(SrcCoordB, 0.0, 0.0);
    
    oColor = vec4(colour.r, colour.r, colour.r, colour.r); // Used for vignette fade.
}
