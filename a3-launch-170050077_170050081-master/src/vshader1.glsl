#version 330

in vec4 vPosition1;
in vec4 vColor1;
in vec3 vNormal1;
in vec2 texCoord1;

out vec4 color;
out vec3 normal;
out vec2 tex;

uniform mat4 uModelViewMatrix;
uniform mat3 normalMatrix;
uniform mat4 viewMatrix;
uniform float texExist1;
uniform float lx, ly, lz;

out float texExist;
out vec4 eye;
out vec4 lightPos;

void main (void) 
{
  gl_Position = uModelViewMatrix * vPosition1;
  
  normal = normalize(normalMatrix * normalize(vNormal1));
  color = vColor1;
  tex = texCoord1;
  texExist = texExist1;
  eye = -(uModelViewMatrix * gl_Position);
  lightPos = vec4(lx, ly, lz, 0.0);
}
