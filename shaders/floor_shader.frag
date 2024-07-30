#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

out vec4 FragColor;

uniform sampler2D texture1;
uniform float alpha;

void main()
{
 	FragColor = vec4(texture(texture1, TexCoords).rgb, alpha);

};