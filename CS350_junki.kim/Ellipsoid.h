/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Ellipsoid.h
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

class Ellipsoid : public BVolume
{
public:
  Ellipsoid(const glm::vec3 _maxPosition, const glm::vec3 _minPosition, Object* pObj) : BVolume(_maxPosition, _minPosition), pSphere(nullptr)
  {
    bvType = BVT_Ellipsoid;

    glm::vec3 centerPos(0.f);
    glm::vec3 radius(0.f);
    glm::mat3 rotationMat3;

    // to scan all vertices to check wether a point in in the sphere or not
    for (auto& mesh : pObj->pModel->meshes)
    {
      glm::vec3 eachCenterPos(0.f);
      glm::vec3 eachRadius(0.f);

      CalcuatingPCA3Axises(mesh.vertices, eachCenterPos, eachRadius, rotationMat3);

      centerPos += eachCenterPos;
      radius += eachRadius;
      // rotation...
    }

    centerPos /= pObj->pModel->meshes.size();
    radius /= pObj->pModel->meshes.size();

    // matrix
    Mat4Identity(rotationMat4);
    for (int i = 0; i < 3; ++i)
      for (int j = 0; j < 3; ++j)
        rotationMat4[i][j] = rotationMat3[i][j];

    pSphere = new Object(new Model("../Common/models/sphere.obj"));
    pSphere->position = glm::vec3(centerPos + pObj->position);
    pSphere->scale = (radius * 2.f) * pObj->scale;
  }

  ~Ellipsoid()
  {
    delete pSphere;
    pSphere = nullptr;
  }

  void Draw(Shader* pShader)
  {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, pSphere->position);
    model = model * rotationMat4; // rotation
    model = glm::scale(model, pSphere->scale);
    pShader->setMat4("model", model);

    pSphere->Draw(pShader);
  }

private:
  Object* pSphere;
  glm::mat4 rotationMat4;
};