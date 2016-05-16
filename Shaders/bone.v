#version 130

varying vec3 N;
varying vec3 v;


in int gl_VertexID;
uniform mat4 boneMatrix[4];
uniform mat4 worldMatrix;
uniform float boneWeight[430];
uniform int boneID[430];

void main(void)
{
  N = normalize(gl_NormalMatrix * gl_Normal);
  vec4 newVertex = gl_Vertex;
  int bID = boneID[gl_VertexID];

  newVertex =  (boneMatrix[0] * worldMatrix * vec4(gl_Vertex.xyz, 1.0));

//  newVertex =  ((boneMatrix[0] * boneWeight[gl_VertexID]) * worldMatrix * vec4(gl_Vertex.xyz, 1.0));
//  newVertex += ((boneMatrix[bID] * (1.0 - boneWeight[gl_VertexID])) * worldMatrix * vec4(gl_Vertex.xyz, 1.0));
  v = vec3(gl_ModelViewMatrix * newVertex);
  gl_Position = (gl_ModelViewProjectionMatrix * vec4(newVertex.xyz, 1.0));
  gl_TexCoord[0] = gl_MultiTexCoord0;
}
