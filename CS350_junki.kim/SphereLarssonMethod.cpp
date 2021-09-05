/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: SphereLarssonMethod.cpp
Purpose: This method works by taking a set of s direction vectors and
         projecting all points onto each vector in s; s serves as a
         speed-accuracy trade-off variable.
Language: C++, Microsoft C++ compiler
Platform: Visual Studio 2019, x64, Window10
Project: CS350_junki.kim
Author: junki.kim
Creation date: 7/7/2021
End Header --------------------------------------------------------*/
#include <glm/glm.hpp>
#include <vector>

#include "SphereLarssonMethod.h"


glm::vec3 LARSSON_NORMALS[49] =
{
  glm::vec3(1, 0, 0),
  glm::vec3(0, 1, 0),
  glm::vec3(0, 0, 1),

  glm::vec3(1, 1, 1),
  glm::vec3(1, 1, -1),
  glm::vec3(1, -1, 1),
  glm::vec3(1, -1, -1),

  glm::vec3(1, 1, 0),
  glm::vec3(1, -1, 0),
  glm::vec3(1, 0, 1),
  glm::vec3(1, 0, -1),
  glm::vec3(0, 1, 1),
  glm::vec3(0, 1, -1),

  glm::vec3(0, 1, 2),
  glm::vec3(0, 2, 1),
  glm::vec3(1, 0, 2),
  glm::vec3(2, 0, 1),
  glm::vec3(1, 2, 0),
  glm::vec3(2, 1, 0),
  glm::vec3(0, 1, -2),
  glm::vec3(0, 2, -1),
  glm::vec3(1, 0, -2),
  glm::vec3(2, 0, -1),
  glm::vec3(1, -2, 0),
  glm::vec3(2, -1, 0),

  glm::vec3(1, 1, 2),
  glm::vec3(2, 1, 1),
  glm::vec3(1, 2, 1),
  glm::vec3(1, -1, 2),
  glm::vec3(1, 1, -2),
  glm::vec3(1, -1, -2),
  glm::vec3(2, -1, 1),
  glm::vec3(2, 1, -1),
  glm::vec3(2, -1, -1),
  glm::vec3(1, -2, 1),
  glm::vec3(1, 2, -1),
  glm::vec3(1, -2, -1),

  glm::vec3(2, 2, 1),
  glm::vec3(1, 2, 2),
  glm::vec3(2, 1, 2),
  glm::vec3(2, -2, 1),
  glm::vec3(2, 2, -1),
  glm::vec3(2, -2, -1),
  glm::vec3(1, -2, 2),
  glm::vec3(1, 2, -2),
  glm::vec3(1, -2, -2),
  glm::vec3(2, -1, 2),
  glm::vec3(2, 1, -2),
  glm::vec3(2, -1, -2)
};

SphereLarssonMethod::SphereLarssonMethod(const glm::vec3 _maxPosition, const glm::vec3 _minPosition, Object* pObj) : BVolume(_maxPosition, _minPosition), pSphere(nullptr)
{
  bvType = BVT_SphereLarssonMethod;

  glm::vec3 centerPos(0.f);
  float radius = 0.f;

  // to scan all vertices to check wether a point in in the sphere or not
  for (auto& mesh : pObj->pModel->meshes)
  {
    glm::vec3 eachCenterPos(0.f);
    float eachRadius = 0.f;

    std::vector<glm::vec3> projectedPoints;

    const size_t maxIterNum = glm::min(mesh.vertices.size(), size_t(49));
    for (unsigned i = 0; i < maxIterNum; ++i)
    {
      float minPproj = FLT_MAX;
      float maxProj = -FLT_MAX;

      unsigned minIndex = 0;
      unsigned maxIndex = 0;

      for (unsigned j = 0; j < mesh.vertices.size(); ++j)
      {
        const glm::vec3& p = mesh.vertices[j].Position;

        float proj = glm::dot(p, LARSSON_NORMALS[i]);

        if (proj < minPproj)
        {
          minPproj = proj;
          minIndex = i;
        }

        if (proj > maxProj)
        {
          maxProj = proj;
          maxIndex = i;
        }
      }

      // store points
      projectedPoints.push_back(mesh.vertices[minIndex].Position);
      projectedPoints.push_back(mesh.vertices[maxIndex].Position);
    }

    //////////////////////////////////////////
    // Ritter's method

    BVSphere sphere;
    sphere.InitSphereMinMaxWithVec3(projectedPoints);
    for (const glm::vec3& p : projectedPoints)
      sphere.UpdateSphere(p);

    //////////////////////////////////////////

    for (const auto& vertex : mesh.vertices)
      sphere.UpdateSphere(vertex.Position);

    centerPos += sphere.center;
    radius += sphere.radius;
  }

  centerPos /= pObj->pModel->meshes.size();
  radius /= pObj->pModel->meshes.size();

  pSphere = new Object(new Model("../Common/models/sphere.obj"));
  pSphere->position = glm::vec3(centerPos + pObj->position);
  pSphere->scale = glm::vec3(radius * 2.f * pObj->scale);
}

SphereLarssonMethod::~SphereLarssonMethod()
{
  delete pSphere;
  pSphere = nullptr;
}

void SphereLarssonMethod::Draw(Shader* pShader)
{
  glm::mat4 model = glm::mat4(1.0f);
  model = glm::translate(model, pSphere->position);
  model = glm::scale(model, pSphere->scale);
  pShader->setMat4("model", model);

  pSphere->Draw(pShader);
}