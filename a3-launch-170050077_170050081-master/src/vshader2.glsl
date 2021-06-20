#version 440

in vec4 vPosition2;
in vec4 vColor2;
out vec4 color;
uniform mat4 uModelViewMatrix;

void main (void) 
{
  gl_Position = uModelViewMatrix * vPosition2;
  color = vColor2;
}
