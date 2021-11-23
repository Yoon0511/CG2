#version 330

in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 objectColor;
uniform mat4 lightmodel;

void main()
{
   float ambientLight = 0.3;
   vec3 ambient = ambientLight * lightColor;

   vec3 normalVector = normalize(Normal);
   vec3 a = lightPos;
   a = vec3(lightmodel * vec4(a,1.0));
   vec3 lightDir = normalize(a  - FragPos);
   float diffuseLight = max(dot(normalVector, lightDir), 0.0);
   vec3 result = (diffuseLight + ambientLight) * lightColor * objectColor;

   FragColor = vec4(result, 1.0);
}