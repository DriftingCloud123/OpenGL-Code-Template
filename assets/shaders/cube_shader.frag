#version 330 core
out vec4 FragColor;

uniform vec3 lightColor;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emission;
    float     shininess;
}; 

struct Light {
    // 定向光&平行光
    // vec3 direction;
    vec3 position;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    // 衰减系数
    float constant;
    float linear;
    float quadratic;
};

uniform Material material;
uniform Light light;
uniform vec3 viewPos;

// 纹理坐标
in vec2 TexCoords;

in vec3 FragPos;
in vec3 Normal;

void main()
{    
    // 环境光
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));

    // 漫反射 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position-FragPos);
    float diff = max(dot(norm, lightDir), 0.0);

    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));

    // 镜面光
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * (spec * vec3(texture(material.specular, TexCoords)));

    // 光线衰减
    float distance    = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + 
                    light.quadratic * (distance * distance));

    vec3 result = (ambient + diffuse + specular) * attenuation;
    FragColor = vec4(result, 1.0);
}


/*
void main(){
    // 环境光
    vec3 ambient = light.ambient * material.ambient;

    // 漫反射
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    vec3 diffuse  = light.diffuse * (diff * material.diffuse); or
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));

    // 镜面光
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * (spec * vec3(texture(material.diffuse, TexCoords)));

    vec3 emission = texture(material.emission, TexCoords).rgb;
    vec3 result = ambient + diffuse + specular;
    // FragColor = vec4(result, 1.0) + vec4(emission, 0.3);
    FragColor = vec4(result, 1.0);
}
*/
