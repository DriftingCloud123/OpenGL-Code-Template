#version 330 core
out vec4 FragColor;

/*命名规则
 *texture_diffuseN: 漫反射纹理
 *texture_specularN: 高光纹理
 *texture_normalN: 法线纹理
 *texture_heightN: 高度纹理
 */

struct Light {
    // 定向光&平行光
    vec3 direction;
    // 点光源
    // vec3 position;
    // 聚光
    // vec3 position;
    // vec3 direction;
    // float cutOff;    // 切光角 的余弦值
    // float outerCutOff;   // 外切光角 的余弦值
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    // 衰减系数
    float constant;
    float linear;
    float quadratic;
};
uniform Light light;
uniform float shininess;

// 材质属性，由model自动传入
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;

// 观察者位置
uniform vec3 viewPos;

// 纹理坐标，由vertex shader传入
in vec2 TexCoords;

// 片段位置和法线位置，由vertex shader传入
in vec3 FragPos;
in vec3 Normal;

void main()
{
    vec3 lightDir = normalize(light.direction);

    // 环境光
    vec3 ambient = light.ambient * vec3(texture(texture_diffuse1, TexCoords));

    // 漫反射 
    vec3 norm = normalize(Normal);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * vec3(texture(texture_diffuse1, TexCoords));

    // 镜面光
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = light.specular * (spec * vec3(texture(texture_specular1, TexCoords)));

    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}