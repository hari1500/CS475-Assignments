#version 440

in vec4 vPosition;

out vec4 color;

uniform mat4 uModelViewMatrix;
uniform vec4 vColor;

void main (void) 
{
  gl_Position = uModelViewMatrix * vPosition;
  color = vColor;
}
