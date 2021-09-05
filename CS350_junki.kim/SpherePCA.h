/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: SpherePCA.h
Purpose: Bouding sphere with Principle Component Analysis.
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

class SpherePCA : public BVolume
{
public:
  SpherePCA(const glm::vec3 _maxPosition, const glm::vec3 _minPosition, Object* pObj) : BVolume(_maxPosition, _minPosition), pSphere(nullptr)
  {
    bvType = BVT_SpherePCA;

    glm::vec3 centerPos(0.f);
    float radius = 0.f;

    // to scan all vertices to check wether a point in in the sphere or not
    for (auto& mesh : pObj->pModel->meshes)
    {
      glm::vec3 eachCenterPos(0.f);
      float eachRadius = 0.f;

      CalcuatingPCA(mesh.vertices, eachCenterPos, eachRadius);

      // for each vertex to grow the bounding sphere
      for (auto vertexItr = mesh.vertices.begin(); vertexItr < mesh.vertices.end(); ++vertexItr)
      {
        glm::vec3 toPoint = vertexItr->Position - eachCenterPos;
        float distSQ = glm::dot(toPoint, toPoint);

        // check if point lies outside sphere
        if (distSQ > (eachRadius * eachRadius))
        {
          float dist = glm::sqrt(distSQ);
          float newRadius = (eachRadius + dist) / 2;
          float adjustment = (newRadius - eachRadius) / dist;

          // store new radius and center
          eachCenterPos += toPoint * adjustment;
          eachRadius = newRadius;
        }
      }

      centerPos += eachCenterPos;
      radius += eachRadius;
    }

    centerPos /= pObj->pModel->meshes.size();
    radius /= pObj->pModel->meshes.size();

    pSphere = new Object(new Model("../Common/models/sphere.obj"));
    pSphere->position = glm::vec3(centerPos + pObj->position);
    pSphere->scale = glm::vec3(radius * 2.f * pObj->scale);
  }

  ~SpherePCA()
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

  void CalcuatingPCA(std::vector<Vertex>& vertices, glm::vec3& centerPos, float& radius)
  {
    glm::mat3 m = Covariance(vertices);
    glm::mat3 v;
    JacobianMatrix(m, v);

    int maxC = 0;
    float maxE = abs(m[0][0]);

    // find component with largest eigenvalue
    float maxF = abs(m[1][1]);
    if (maxF > maxE)
      maxC = 1;

    maxF = abs(m[2][2]);
    if (maxF > maxE)
      maxC = 2;

    glm::vec3 direction(v[0][maxC], v[1][maxC], v[2][maxC]);

    // get farest two points along the given direction
    glm::vec3 minPosFPAD, maxPosFPAD;
    FarestPointsAlongDir(direction, vertices, minPosFPAD, maxPosFPAD);

    float dist = glm::sqrt(DistanceSQ(minPosFPAD, maxPosFPAD));

    // update
    centerPos += (minPosFPAD + maxPosFPAD) / 2.0f;
    radius += dist / 2.0f;
  }

private:
  Object* pSphere;
};