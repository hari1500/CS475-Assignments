#version 400

in vec4 color;
in vec3 normal;
in vec2 tex;
in float texExist;
in vec4 eye;
in vec4 lightPos;

uniform mat4 viewMatrix;
uniform sampler2D texture;
out vec4 frag_color;

void main () 
{
  // Defining Materials
  vec4 diffuse = vec4(0.65, 0.65, 0.65, 1.0); 
  vec4 ambient = vec4(0.15, 0.15, 0.01, 1.0);
  vec4 specular = vec4(0.5, 0.5, 0.5, 1.0);
  float shininess = 0.01;
  vec4 spec = vec4(0.5); 
  
  // Defining Light 
  vec3 lightDir = vec3(viewMatrix * lightPos); 
  lightDir = normalize(lightDir);  

  float dotProduct = dot(normal, lightDir);
  float intensity =  max( dotProduct, 0.0);

  // Compute specular component only if light falls on vertex
  if(intensity > 0.0)
  {
    vec3 e = normalize(vec3(eye));
    vec3 h = normalize(lightDir + e );
    float intSpec = max(dot(h,normal), 0.01);	
    spec = specular * pow(intSpec, shininess);
  }
  
  if(texExist > 0) frag_color = max((intensity * diffuse  + spec)*texture2D(texture, tex), ambient);
  else frag_color = max((intensity * diffuse  + spec)*color, ambient); // All
  // color = max((intensity * diffuse  + spec)*texture2D(texture, texCoord1), ambient2);
}