/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: SphereRitterMethod.h
Purpose: Ritter's algorithm runs in time O(nd) on inputs consisting of
         n points in d-dimensional space, which makes it very efficient.
         However it gives only a coarse result which is usually 5% to
         20% larger than the optimum.
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

class SphereRitterMethod : public BVolume
{
public:
  SphereRitterMethod(const glm::vec3 _maxPosition, const glm::vec3 _minPosition, Object* pObj) : BVolume(_maxPosition, _minPosition), pSphere(nullptr)
  {
    bvType = BVT_SphereRitterMethod;

    BVSphere bVolumesphere;
    bVolumesphere.InitSphereMinMaxWithVertex(pObj->pModel->meshes.begin()->vertices);

    for (auto& mesh : pObj->pModel->meshes)
    {
      bVolumesphere.InitSphereMinMaxWithVertex(mesh.vertices);

      for (auto& vertex : mesh.vertices)
        bVolumesphere.UpdateSphere(vertex.Position);
    }

    pSphere = new Object(new Model("../Common/models/sphere.obj"));
    pSphere->position = glm::vec3(bVolumesphere.center + pObj->position);
    pSphere->scale = glm::vec3(bVolumesphere.radius * 2.f * pObj->scale);
  }

  ~SphereRitterMethod()
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

    pSphere->Draw(pShader);
  }

private:
  Object* pSphere;
};