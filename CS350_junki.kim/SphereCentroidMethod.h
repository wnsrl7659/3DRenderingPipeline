/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: SphereCentroidMethod.h
Purpose: The radius is the max distance from two vertices.
Language: C++, Microsoft C++ compiler
Platform: Visual Studio 2019, x64, Window10
Project: CS350_junki.kim
Author: junki.kim
Creation date: 7/7/2021
End Header --------------------------------------------------------*/
#pragma once

#include "BVolume.h"
#include "Object.h"

class SphereCentroidMethod : public BVolume
{
public:
  SphereCentroidMethod(const glm::vec3 _maxPosition, const glm::vec3 _minPosition, const float _maxDist) : BVolume(_maxPosition, _minPosition), pSphere(nullptr)
  {
    bvType = BVT_SphereCentroidMethod;
    const glm::vec3 centerPos = (_maxPosition + _minPosition) / 2.f;
    radius = _maxDist;
    color = glm::vec3(1.f, 0.f, 0.f);

    pSphere = new Object(new Model("../Common/models/sphere.obj"));
    pSphere->position = centerPos;
    pSphere->scale = glm::vec3(radius * 2.f);
  }

  SphereCentroidMethod(const glm::vec3 _centerPos, const float _maxDist) : pSphere(nullptr)
  {
    bvType = BVT_SphereCentroidMethod;
    const glm::vec3 centerPos = _centerPos;
    radius = _maxDist;
    color = glm::vec3(1.f, 0.f, 0.f);

    pSphere = new Object(new Model("../Common/models/sphere.obj"));
    pSphere->position = centerPos;
    pSphere->scale = glm::vec3(radius * 2.f);
  }

  ~SphereCentroidMethod()
  {
    delete pSphere;
    pSphere = nullptr;
  }

  void Draw(Shader* pShader)
  {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, pSphere->position);
    model = glm::scale(model, pSphere->scale);
    pShader->setMat4("model", model);
    pShader->setVec3("color", color);

    pSphere->Draw(pShader);
  }

  inline float GetRadius() { return radius; }

private:
  Object* pSphere;
  float radius;
};