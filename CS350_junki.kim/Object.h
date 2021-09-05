/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Object.h
Purpose: It contains essential attributes of an object
Language: C++, Microsoft C++ compiler
Platform: Visual Studio 2019, x64, Window10
Project: CS350_junki.kim
Author: junki.kim
Creation date: 5/21/2021
End Header --------------------------------------------------------*/

#pragma once

#include "Model.h"
#include "BVolume.h"

#include <list>

class Object
{
public:
  Object(
    Model* _pModel, bool needSetupMesh = true,
    glm::vec3 _pos = glm::vec3(0.f), glm::vec3 _scale = glm::vec3(1.f));
  Object(
    Model* _pModel, float _scaleInitOffset, bool needSetupMesh = true,
    glm::vec3 _pos = glm::vec3(0.f), glm::vec3 _scale = glm::vec3(1.f));
  ~Object();

  void SetupMesh();
  void Draw(Shader* pShader);
  void AddBoundingVolumes();

  // Getter
  inline glm::vec3 GetPositionOffset() { return positionOffset; }
  inline glm::vec3 GetScaleOffset() { return scaleOffset; }
  inline glm::vec3 GetMaxPos()
  {
    return ((pModel->GetMaxPos() * scaleInitOffset) - positionOffset) * scaleOffset;
  }
  inline glm::vec3 GetMinPos()
  {
    return ((pModel->GetMinPos() * scaleInitOffset) - positionOffset) * scaleOffset;
  }
  const std::list<BVolume*> GetBVolumeList() const { return pBVolumeList; }
  inline float GetMaxDist() { return maxDist; }
  inline BVolume* GetBVolume(BVType _type)
  {
    for (BVolume* pVolume : pBVolumeList)
    {
      if (pVolume->bvType == _type)
        return pVolume;
    }

    return nullptr;
  }

  glm::vec3 position;
  glm::vec3 scale;
  glm::vec3 sacleOffset;
  glm::vec3 color;
  bool isActivated;

  // These class need to access to Mesh's vertices
  friend class SphereRitterMethod;
  friend class SphereLarssonMethod;
  friend class SpherePCA;
  friend class Ellipsoid;
  friend class BoxOBB;
  friend class HierarchyTopDownAABB;
  friend class HierarchyTopDownSphere;
  friend class AdaptiveOctree;
  friend class BSPTree;

private:
  Model* pModel;
  std::list<BVolume*> pBVolumeList;

  glm::vec3 positionOffset; // unit position (the origin of a Euclidean space)
  glm::vec3 scaleOffset;    // unit size (fits in 1 size of a cube)
  float scaleInitOffset;
  float maxDist; // distance with the farest vertex from Zero
  float minDist; // distance with the nearest vertex from Zero
  glm::vec3 farestVertexA;      // Ritter: the first vertex
  glm::vec3 farestVertexB;      // Ritter: the farest vertex with A
  float maxDistanceSQBetweenAB; // Ritter

  // Bounding Volumes
  void AddBoxAABB();
  void AddSphereCentroidMethod();
  void AddSphereRitterMethod();
  void AddSphereLarssonMethod();
  void AddSpherePCA();
  void AddEllipsoid();
  void AddBoxOBB();
};