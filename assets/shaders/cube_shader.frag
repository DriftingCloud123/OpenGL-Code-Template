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
    // 点光源
    // vec3 position;
    // 聚光
    vec3 position;
    vec3 direction;
    float cutOff;    // 切光角 的余弦值
    float outerCutOff;   // 外切光角 的余弦值
    
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
    vec3 lightDir = normalize(light.position - FragPos);
    float theta = dot(lightDir, normalize(-light.direction));

    // 环境光
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));

    if(theta > light.outerCutOff) {
        // 光线衰减
        float distance    = length(light.position - FragPos);
        float attenuation = 1.0 / (light.constant + light.linear * distance + 
                        light.quadratic * (distance * distance));

        // 聚光平滑
        float epsilon = light.cutOff - light.outerCutOff;
        float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

        // 漫反射 
        vec3 norm = normalize(Normal);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));

        // 镜面光
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 reflectDir = reflect(-lightDir, norm);  
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        vec3 specular = light.specular * (spec * vec3(texture(material.specular, TexCoords)));

        diffuse *= attenuation;
        specular *= attenuation;

        diffuse *= intensity;
        specular *= intensity;

        vec3 result = ambient + diffuse + specular;
        FragColor = vec4(result, 1.0);
    } else {
        FragColor = vec4(ambient, 1.0);
    }

}

// void main()
// {    
//     // 环境光
//     vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));

//     // 漫反射 
//     vec3 norm = normalize(Normal);
//     vec3 lightDir = normalize(light.position-FragPos);
//     float diff = max(dot(norm, lightDir), 0.0);

//     vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));

//     // 镜面光
//     vec3 viewDir = normalize(viewPos - FragPos);
//     vec3 reflectDir = reflect(-lightDir, norm);  
//     float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
//     vec3 specular = light.specular * (spec * vec3(texture(material.specular, TexCoords)));

//     // 光线衰减
//     float distance    = length(light.position - FragPos);
//     float attenuation = 1.0 / (light.constant + light.linear * distance + 
//                     light.quadratic * (distance * distance));

//     vec3 result = (ambient + diffuse + specular) * attenuation;
//     FragColor = vec4(result, 1.0);
// }


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
