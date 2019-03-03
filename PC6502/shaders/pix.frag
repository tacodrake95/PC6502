#version 330 core
out vec4 color;

in vec2 TexCoords;

uniform sampler2D screenTexture;

void main()
{
    color = texture(screenTexture, vec2((1 + TexCoords.x)/2, (1 - TexCoords.y) / 2));
} 