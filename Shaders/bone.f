uniform sampler2D textureIn;
uniform int isOutline;

varying vec3 N;
varying vec3 v;

void main (void)  
{
  if (isOutline == 1) {
    gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);
  } else {
    vec3 L = normalize(gl_LightSource[0].position.xyz - v);   
    vec3 E = normalize(-v); // we are in Eye Coordinates, so EyePos is (0,0,0)  
    vec3 R = normalize(-reflect(L,N));  
    vec4 Iamb = gl_FrontLightProduct[0].ambient;    
    vec4 Idiff = gl_FrontLightProduct[0].diffuse * max(dot(N,L), 0.0);
    Idiff = clamp(Idiff, 0.0, 1.0);     
    vec4 Ispec = gl_FrontLightProduct[0].specular * pow(max(dot(R,E),0.0),0.3*gl_FrontMaterial.shininess);
    Ispec = clamp(Ispec, 0.0, 1.0); 
    gl_FragColor = texture2D(textureIn, gl_TexCoord[0].xy).rgba *
    ((gl_FrontLightModelProduct.sceneColor + Iamb + Idiff + Ispec) / 2 + 0.5); 
  }
}