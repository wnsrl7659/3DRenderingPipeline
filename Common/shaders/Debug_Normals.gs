/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Debug_Normals.gs
Purpose: It shows vertex normals or face normals in Geometry Shader stage
Language: C++, Microsoft C++ compiler
Platform: Visual Studio 2019, x64, Window10
Project: CS350_junki.kim
Author: junki.kim
Creation date: 5/21/2021
End Header --------------------------------------------------------*/

//#version 330 core
//layout (triangles) in;
//layout (line_strip, max_vertices = 6) out;

in VS_OUT {
    vec3 Normal;
} gs_in[];

const float MAGNITUDE = 0.1f;

uniform bool showVertexNormal;
uniform bool showFaceNormal;

void GenerateLine(int index)
{
    gl_Position = gl_in[index].gl_Position; // start point
    EmitVertex();
    gl_Position = gl_in[index].gl_Position + vec4(gs_in[index].Normal, 0.0) * MAGNITUDE; // end point
    EmitVertex();
    EndPrimitive();
}

void main()
{
    // vertex normal
    if(showVertexNormal)
    {
      GenerateLine(0);
      GenerateLine(1);
      GenerateLine(2);
    }

    // face normal
    if(showFaceNormal)
    {
      // the mid point between the 3 points (face)
      vec4 midPosition = (gl_in[0].gl_Position + gl_in[1].gl_Position + gl_in[2].gl_Position) / 3.f;
      vec3 midNormal = (gs_in[0].Normal + gs_in[1].Normal + gs_in[2].Normal) / 3.f;
    
      gl_Position = midPosition;
      EmitVertex();
      gl_Position = midPosition + vec4(midNormal, 0.0) * MAGNITUDE;
      EmitVertex();
      EndPrimitive();
    }
}