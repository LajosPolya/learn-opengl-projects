#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 LightingColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 viewPos;
  
uniform vec3 lightPos;
uniform vec3 lightColor;

// Gouraud shading
void main()
{
    vec3 Pos = vec3(model * vec4(aPos, 1.0));
    // Inversing a matrix is expensive in a shader and should be done on the CPU
    vec3 Normal = mat3(transpose(inverse(model))) * aNormal;    
    
    gl_Position = projection * view * vec4(Pos, 1.0);

    // ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - Pos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - Pos);
    vec3 reflectDir = reflect(-lightDir, norm);
    // 32 is the inensity of the specular lighting,
    // a lower number scatters the light more and a 
    // higher number condenses it more (should be power of 2?)
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    LightingColor = (ambient + diffuse + specular);
}