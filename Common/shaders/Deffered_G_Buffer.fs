/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Deffered_G_Buffer.fs
Purpose: It's a geometry pass phase
Language: C++, Microsoft C++ compiler
Platform: Visual Studio 2019, x64, Window10
Project: CS350_junki.kim
Author: junki.kim
Creation date: 5/21/2021
End Header --------------------------------------------------------*/

#version 330 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec; // texture color

in VS_OUT
{
    vec3 FragPos;
    vec2 TexCoords;
    vec3 Normal;
    vec3 NodeColor;
} fs_in;

uniform sampler2D texture_diffuse;  // not using yet 
uniform sampler2D texture_specular; // not using yet 
uniform vec3 color; // an additional user-defined color
uniform int renderTarget;

void main()
{    
    gPosition = fs_in.FragPos;         // vertex position
    gNormal = normalize(fs_in.Normal); // vertex normals

    // FSQ_RenderTarget
    if(renderTarget == 0) // GEOMETRY
    {
        // white color
        gAlbedoSpec.rgb = vec3(1.f, 1.f, 1.f);
        gAlbedoSpec.a = 1.f;
    }
    else if(renderTarget == 6) // Each Node Color (Spatial Partitioning)
    {
        gAlbedoSpec.rgb = fs_in.NodeColor;
        gAlbedoSpec.a = 1.f; 
    }
    else // NON GEOMETRY
    {
        gAlbedoSpec.rgb = color; // color from its texture or the user
        gAlbedoSpec.a = 1.f;     // specular intensity from its texture or the user
    }
}