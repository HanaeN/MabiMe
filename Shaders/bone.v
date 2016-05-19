varying vec3 N;
varying vec3 v;
varying vec2 tex;
attribute vec3 vertexPos;
attribute vec3 vertexNormal;
attribute vec2 vertexUV;

//in int gl_VertexID;
//uniform mat4 boneMatrix[4];
uniform mat4 worldMatrix;
//uniform float boneWeight[430];
//uniform int boneID[430];

void main(void)
{
  N = normalize(gl_NormalMatrix * vertexNormal);
  vec4 newVertex = worldMatrix * vec4(vertexPos.xyz, 1.0);
//  int bID = boneID[gl_VertexID];
//  newVertex =  ((boneMatrix[0] * boneWeight[gl_VertexID]) * worldMatrix * vec4(gl_Vertex.xyz, 1.0));
//  newVertex += ((boneMatrix[0] * (1.0 - boneWeight[gl_VertexID])) * worldMatrix * vec4(gl_Vertex.xyz, 1.0));
  v = vec3(gl_ModelViewMatrix * newVertex);
//  gl_Position = (gl_ModelViewProjectionMatrix * vec4(newVertex.xyz, 1.0));
  gl_Position = (gl_ModelViewProjectionMatrix * vec4(newVertex.xyz, 1.0));
  tex = vertexUV;
}
