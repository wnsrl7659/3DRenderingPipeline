/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Deffered_FSQ_Buffer.fs
Purpose: Using the G-buffer, it makes Phong Shading on a Full Screen Quad
Language: C++, Microsoft C++ compiler
Platform: Visual Studio 2019, x64, Window10
Project: CS350_junki.kim
Author: junki.kim
Creation date: 5/21/2021
End Header --------------------------------------------------------*/

#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

struct Light {
    vec3 Position;
    vec3 Color;

    float Linear;
    float Quadratic;
};

const int MAX_LIGHTS = 16;
uniform Light lights[MAX_LIGHTS];
uniform int currLightsNum;
uniform vec3 viewPos;
uniform int renderTarget;

void main()
{             
    // Data from G-Buffer
    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 Normal = texture(gNormal, TexCoords).rgb;
    vec3 Diffuse = texture(gAlbedoSpec, TexCoords).rgb;
    float Specular = texture(gAlbedoSpec, TexCoords).a;
    
    // for the final outout data
    vec3 out_lighting = Diffuse * 0.1f; // material ambient
    vec3 out_specular = vec3(0.f, 0.f, 0.f); 
    vec3 out_diffuse = vec3(0.f, 0.f, 0.f); 

    vec3 viewDir  = normalize(viewPos - FragPos);

    if(renderTarget != 0) // Geometry Mode dosen't need the light calculation
    {
        // Light Calculation (Phong Shading)
        for(int i = 0; i < currLightsNum; ++i)
        {
            // diffuse
            vec3 lightDir = normalize(lights[i].Position - FragPos);
            vec3 diffuse = max(dot(Normal, lightDir), 0.f) * Diffuse * lights[i].Color;

            // specular
            vec3 halfwayDir = normalize(lightDir + viewDir);  
            float spec = pow(max(dot(Normal, halfwayDir), 0.f), 256.f);
            vec3 specular = lights[i].Color * spec * Specular;

            // attenuation
            float distance = length(lights[i].Position - FragPos);
            float attenuation = 1.f / (1.f + lights[i].Linear * distance + lights[i].Quadratic * distance * distance);

            diffuse *= attenuation;
            specular *= attenuation;

            out_lighting += diffuse + specular;
            out_specular += specular;
            out_diffuse += diffuse;
        }
    }

    // FSQ_RenderTarget
    switch (renderTarget)
    {
        case 0: // Geometry
            FragColor = vec4(Diffuse, 1.f);
            break;
        case 1: // Position
            FragColor = vec4(FragPos, 1.f);
            break;
        case 2: // Normals
            FragColor = vec4(Normal, 1.f);
            break;
        case 3: // Albedo
            FragColor = vec4(out_diffuse, 1.f);
            break;
        case 4: // Specular (emphasized)
            FragColor = vec4(out_specular * 2.f, 1.f);
            break;
        case 5: // Lighting
             FragColor = vec4(out_lighting, 1.f);
            break;
        case 6: // Each Node Color (Spatial Partitioning)
            FragColor = vec4(Diffuse, 1.f);
            break;
        default: // blue screen for an unintented vlaue
            FragColor = vec4(0.f, 0.f, 1.f, 1.f);
            break;
    }
}