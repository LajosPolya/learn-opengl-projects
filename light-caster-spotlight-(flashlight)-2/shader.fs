#version 430 core
out vec4 FragColor;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float     shininess;
}; 
  
uniform Material material;

struct Light {
    vec3 position;
    vec3 direction;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;

    float cutOff;
    float outerCutOff;
};

uniform Light light; 

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoord;
uniform vec3 viewPos;

void main()
{
    
    vec3 lightDir = light.position - FragPos;
    vec3 lightDirNorm = normalize(lightDir);

    float theta = dot(lightDirNorm, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    // attentuation
    float distance = length(lightDir);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    // ambient
    vec3 ambient = light.ambient * texture(material.diffuse, TexCoord).rgb;

    // diffuse 
    vec3 norm = normalize(Normal);
    float diff = max(dot(norm, lightDirNorm), 0.0);
    vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoord).rgb;
    diffuse *= attenuation;
    diffuse *= intensity;

    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDirNorm, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * (spec * texture(material.specular, TexCoord).rgb);
    specular *= attenuation;
    specular *= intensity;
            
    vec3 result = (ambient + diffuse + specular);
    FragColor = vec4(result, 1.0);
} 