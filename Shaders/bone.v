varying vec3 N;
varying vec3 v;
varying vec2 tex;
attribute vec3 vertexPos;
attribute vec3 vertexNormal;
attribute vec2 vertexUV;
attribute vec2 boneWeight;
attribute float boneID;

uniform mat4 boneMatrix[4];

void main(void)
{
  N = normalize(gl_NormalMatrix * vertexNormal);
  vec4 newVertex;
  newVertex =  ((boneMatrix[0] * vec4(vertexPos.xyz, 1.0)) * boneWeight.x);
  newVertex += ((boneMatrix[int(boneID)] * vec4(vertexPos.xyz, 1.0)) * boneWeight.y);
  v = vec3(gl_ModelViewMatrix * newVertex);
  gl_Position = (gl_ModelViewProjectionMatrix * vec4(newVertex.xyz, 1.0));
  tex = vertexUV;
}
