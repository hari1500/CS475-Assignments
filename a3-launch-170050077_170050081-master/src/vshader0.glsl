#version 330

in vec4 vPosition0;
in vec2 texCoord0;
in vec3 vNormal0;

out vec2 tex;
uniform mat4 uModelViewMatrix;
uniform mat3 normalMatrix;
uniform mat4 viewMatrix;

void main (void) 
{
  gl_Position = uModelViewMatrix * vPosition0;
  tex = texCoord0;
}
