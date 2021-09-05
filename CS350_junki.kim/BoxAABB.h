/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: BoxAABB.h
Purpose: The axis-aligned minimum bounding box (or AABB) for a given
         point set is its minimum bounding box subject to the constraint
         that the edges of the box are parallel to the (Cartesian) coordinate
         axes.
Language: C++, Microsoft C++ compiler
Platform: Visual Studio 2019, x64, Window10
Project: CS350_junki.kim
Author: junki.kim
Creation date: 7/7/2021
End Header --------------------------------------------------------*/

#pragma once

#include "BVolume.h"
#include <array>

class BoxAABB : public BVolume
{
public:
  unsigned int VAO;
  BoxAABB(const std::vector<glm::vec3>& vertices)
  {
    bvType = BVT_BoxAABB;

    glm::vec3 _maxAxis(-FLT_MAX);
    glm::vec3 _minAxis(FLT_MAX);

    // find min and max points of vertices
    for (unsigned i = 0; i < vertices.size(); ++i)
    {
      const glm::vec3& vertex = vertices[i];

      // find max
      _maxAxis.x = max(_maxAxis.x, vertex.x);
      _maxAxis.y = max(_maxAxis.y, vertex.y);
      _maxAxis.z = max(_maxAxis.z, vertex.z);

      // find min
      _minAxis.x = min(_minAxis.x, vertex.x);
      _minAxis.y = min(_minAxis.y, vertex.y);
      _minAxis.z = min(_minAxis.z, vertex.z);
    }

    InitOpenGLAttributes(_maxAxis, _minAxis);
  }

  BoxAABB(const glm::vec3 _maxPosition, const glm::vec3 _minPosition) : BVolume(_maxPosition, _minPosition)
  {
    bvType = BVT_BoxAABB;

    InitOpenGLAttributes(_maxPosition, _minPosition);
  }

  ~BoxAABB() {};

  void Draw(Shader* pShader)
  {
    // reset the model coordinate, AABB doesn't have own position and scale value
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.f));
    model = glm::scale(model, glm::vec3(1.f));
    pShader->setMat4("model", model);
    pShader->setVec3("color", color);

    // draw
    glBindVertexArray(VAO);
    glDrawArrays(GL_LINES, 0, verPos.max_size());
    glBindVertexArray(0);
  }

private:
  std::array<glm::vec3, 24> verPos; // 24 = 6 faces * 4 vertices
  unsigned int VBO;

  void InitOpenGLAttributes(const glm::vec3 _maxPosition, const glm::vec3 _minPosition)
  {
    // 8 vertices for a cube
    const glm::vec3 vertices[8] = {
      _maxPosition,
      glm::vec3(_minPosition.x, _maxPosition.y, _maxPosition.z),
      glm::vec3(_minPosition.x, _minPosition.y, _maxPosition.z),
      glm::vec3(_maxPosition.x, _minPosition.y, _maxPosition.z),
      glm::vec3(_maxPosition.x, _minPosition.y, _minPosition.z),
      glm::vec3(_maxPosition.x, _maxPosition.y, _minPosition.z),
      glm::vec3(_minPosition.x, _maxPosition.y, _minPosition.z),
      _minPosition
    };

    // front face
    verPos[0] = vertices[0];
    verPos[1] = vertices[1];
    verPos[2] = vertices[1];
    verPos[3] = vertices[2];
    verPos[4] = vertices[2];
    verPos[5] = vertices[3];
    verPos[6] = vertices[3];
    verPos[7] = vertices[0];

    // right face
    verPos[8] = vertices[0];
    verPos[9] = vertices[5];
    verPos[10] = vertices[5];
    verPos[11] = vertices[4];
    verPos[12] = vertices[4];
    verPos[13] = vertices[3];

    // bottom face
    verPos[14] = vertices[4];
    verPos[15] = vertices[7];
    verPos[16] = vertices[7];
    verPos[17] = vertices[2];

    // remained face
    verPos[18] = vertices[6];
    verPos[19] = vertices[1];
    verPos[20] = vertices[6];
    verPos[21] = vertices[7];
    verPos[22] = vertices[6];
    verPos[23] = vertices[5];

    centerPosition = (_maxPosition + _minPosition) / 2.f;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, verPos.max_size() * sizeof(glm::vec3), &verPos[0], GL_STATIC_DRAW);

    // positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

    glBindVertexArray(0);
  }
};