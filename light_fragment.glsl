#version 330

in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 objectColor;

void main()
{
   float ambientLight = 0.3;
   vec3 ambient = ambientLight * lightColor;

   vec3 normalVector = normalize(Normal);
   vec3 lightDir = normalize(lightPos - FragPos);
   float diffuseLight = max(dot(normalVector, lightDir), 0.0);
   vec3 result = (diffuseLight + ambientLight) * lightColor * objectColor;

   FragColor = vec4(result, 1.0);
}