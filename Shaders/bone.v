#version 120

varying vec3 N;
varying vec3 v;

in int gl_VertexID;

uniform mat4x4 boneMatrix;
uniform float boneWeight[1000];

void main(void)
{
  N = normalize(gl_NormalMatrix * gl_Normal);
  vec4 newVertex;
  newVertex = (vec4(gl_Vertex.xyz, 1.0) * (boneMatrix * boneWeight[gl_VertexID]));
  v = vec3(gl_ModelViewMatrix * newVertex);       
  gl_Position = (gl_ModelViewProjectionMatrix * vec4(newVertex.xyz, 1.0));
  gl_TexCoord[0] = gl_MultiTexCoord0;
}