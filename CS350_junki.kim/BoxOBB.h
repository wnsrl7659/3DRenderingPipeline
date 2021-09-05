/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: BoxOBB.h
Purpose: Bounding sphere with Principle Component Analysis.
Language: C++, Microsoft C++ compiler
Platform: Visual Studio 2019, x64, Window10
Project: CS350_junki.kim
Author: junki.kim
Creation date: 7/7/2021
End Header --------------------------------------------------------*/
#pragma once

#include "BVolume.h"
#include "Object.h"
#include "HelperFunctions.h"
#include <array>

class BoxOBB : public BVolume
{
public:
  unsigned int VAO;
  BoxOBB(const glm::vec3 _maxPosition, const glm::vec3 _minPosition, Object* pObj) : BVolume(_maxPosition, _minPosition)
  {
    bvType = BVT_BoxOBB;

    glm::vec3 centerPos(0.f);
    glm::vec3 width(0.f);
    glm::mat3 rotationMat3;

    // to scan all vertices to check wether a point in in the sphere or not
    for (auto& mesh : pObj->pModel->meshes)
    {
      glm::vec3 eachCenterPos(0.f);
      glm::vec3 eachWidth(0.f);

      CalcuatingPCA3Axises(mesh.vertices, eachCenterPos, eachWidth, rotationMat3);

      centerPos += eachCenterPos;
      width += glm::abs(eachWidth);
      // rotation...
    }

    centerPos /= pObj->pModel->meshes.size();
    width /= pObj->pModel->meshes.size();

    // matrix
    Mat4Identity(rotationMat4);
    for (int i = 0; i < 3; ++i)
      for (int j = 0; j < 3; ++j)
        rotationMat4[i][j] = rotationMat3[i][j];

    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    glm::vec3 obbMaxPos = (centerPos + width);
    glm::vec3 obbMinPos = (centerPos - width);

    // 8 vertices for a cube
    const glm::vec3 vertices[8] = {
      obbMaxPos,
      glm::vec3(obbMinPos.x, obbMaxPos.y, obbMaxPos.z),
      glm::vec3(obbMinPos.x, obbMinPos.y, obbMaxPos.z),
      glm::vec3(obbMaxPos.x, obbMinPos.y, obbMaxPos.z),
      glm::vec3(obbMaxPos.x, obbMinPos.y, obbMinPos.z),
      glm::vec3(obbMaxPos.x, obbMaxPos.y, obbMinPos.z),
      glm::vec3(obbMinPos.x, obbMaxPos.y, obbMinPos.z),
      obbMinPos
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

  ~BoxOBB()
  {
  }

  void Draw(Shader* pShader)
  {
    // reset the model coordinate, OBB doesn't have own position and scale value, but rotation!
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.f));
    model = model * rotationMat4; // rotation
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
  glm::mat4 rotationMat4;
};