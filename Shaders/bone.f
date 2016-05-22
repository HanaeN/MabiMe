#extension GL_EXT_gpu_shader4 : enable

uniform sampler2D textureIn;
uniform int isOutline;
uniform vec4 modifyRGB;

varying vec3 N;
varying vec3 v;
varying vec2 tex;

void main (void)
{
  if (isOutline == 1) {
    gl_FragColor = vec4(0.0, 0.0, 0.0, texture2D(textureIn, tex.xy).a);
  } else {
    vec3 L = normalize(gl_LightSource[0].position.xyz - v);
    vec3 E = normalize(-v); // we are in Eye Coordinates, so EyePos is (0,0,0)
    vec3 R = normalize(-reflect(L,N));
    vec4 Iamb = gl_FrontLightProduct[0].ambient;
    vec4 Idiff = gl_FrontLightProduct[0].diffuse * max(dot(N,L), 0.0);
    Idiff = clamp(Idiff, 0.0, 1.0);
    vec4 Ispec = gl_FrontLightProduct[0].specular * pow(max(dot(R,E),0.0),0.3*gl_FrontMaterial.shininess);
    Ispec = clamp(Ispec, 0.0, 1.0);
    vec4 colour = texture2D(textureIn, tex.xy).rgba;
    if (modifyRGB.a > 0) {
        float intensity = 0.3 * colour.r + 0.59 * colour.g + 0.11 * colour.b;
        intensity = intensity - 0.5;
        if (intensity < 0) intensity = 0;
        colour.rgb = ((colour.rgb * 2) * (modifyRGB.rgb * (1.0 - (intensity * 2)))) + (intensity * 2);
    }
    gl_FragColor = colour *
    ((gl_FrontLightModelProduct.sceneColor + Iamb + Idiff + Ispec) / 10.0 + 0.90);
  }
}
