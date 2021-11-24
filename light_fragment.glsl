#version 330

in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 objectColor;
uniform mat4 lightmodel;
uniform vec3 light_on;

void main()
{
	float ambientLight = 0.3;
	vec3 ambient = ambientLight * lightColor;

	vec3 normalVector = normalize(Normal);

	vec3 mlightpos = vec3(lightmodel * vec4(lightPos,1.0));

	vec3 lightdir = normalize(mlightpos - FragPos);
	float diffuseLight = max(dot(normalVector, lightdir), 0.0);
	vec3 diffuse = diffuseLight * lightColor;

	vec3 result = (ambient + diffuse) * objectColor;
	
	if(light_on == vec3(0.0f,0.0f,0.0f))
	{
		FragColor = vec4 (result, 1.0);
	}
	else
	{
		FragColor = vec4 (objectColor, 1.0);
	}
}