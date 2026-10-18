#version 330 core

in vec3 ourColor;   
in vec2 ourTexCoord;

uniform sampler2D aTexture;

out vec4 FragColor;

void main()
{
    // FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
    FragColor = texture(aTexture, ourTexCoord) * vec4(ourColor, 1.0f);
}
