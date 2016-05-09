#version 120

varying vec3 N;
varying vec3 v;

uniform mat4x4 boneMatrix;
uniform mat4x4 worldMatrix;
uniform float boneWeight[1000];

void main(void)
{
  vec4 newVertex;
  v = vec3(gl_ModelViewMatrix * gl_Vertex);       
  N = normalize(gl_NormalMatrix * gl_Normal);
  newVertex = (gl_Vertex * boneMatrix) * worldMatrix;
  gl_Position = gl_ModelViewProjectionMatrix * newVertex;
  gl_TexCoord[0] = gl_MultiTexCoord0;
}       