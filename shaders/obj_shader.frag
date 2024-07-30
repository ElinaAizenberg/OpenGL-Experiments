#version 330 core

out vec4 FragColor;

struct Material {
    sampler2D diffuse;
    vec3 ambient;
    vec3 diffuse_vec;
    vec3 specular;
    float shininess;
};

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 ObjectColor;
uniform bool isTexture1Provided;
uniform sampler2D texture1;

uniform vec3 viewPos;
uniform Material material;
uniform Light light;

uniform int buffer_id;

void renderWithObjectColor()
{
    FragColor = vec4(ObjectColor, 1.0);
}

void renderWithLighting(vec3 ambient_vector, vec3 diffuse_vector)
{
    // ambient
    vec3 ambient = light.ambient * ambient_vector;

    // diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * diffuse_vector;

    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * (spec * material.specular);

    vec3 result = ambient + diffuse + specular;

    FragColor = vec4(result, 1.0);
}


void main()
{
    vec3 result;
    if (buffer_id == 0)
    {
        if (isTexture1Provided)
        {
            renderWithLighting(texture(material.diffuse, TexCoords).rgb, texture(material.diffuse, TexCoords).rgb);
        }
        else
        {
            renderWithLighting(material.ambient, material.diffuse_vec);
        }
    }
    else if (buffer_id == 1)
    {
        renderWithObjectColor();
    }
}