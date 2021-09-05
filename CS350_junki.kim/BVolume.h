/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: BVolume.h
Purpose: This is a base class for all types of bounding volumes.
Language: C++, Microsoft C++ compiler
Platform: Visual Studio 2019, x64, Window10
Project: CS350_junki.kim
Author: junki.kim
Creation date: 7/7/2021
End Header --------------------------------------------------------*/

#pragma once

#include "Mesh.h"

#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "../Common/Shader.hpp"

enum BVType
{
  BVT_None = 0,

  // Axis	Aligned	Boxes
  BVT_BoxAABB,

  // Bounding	Sphere
  BVT_SphereCentroidMethod,
  BVT_SphereRitterMethod,
  BVT_SphereLarssonMethod,
  BVT_SpherePCA,

  // Bounding	Ellipsoid
  BVT_Ellipsoid,

  // Oriented	Boxes
  BVT_BoxOBB
};

struct BVSphere
{
public:
  glm::vec3 center;
  float radius;
  glm::vec3 maxVertex;
  glm::vec3 minVertex;
  glm::vec3 maxAxis;
  glm::vec3 minAxis;

  void InitSphereMinMaxWithVec3(const std::vector<glm::vec3>& _posVec);
  void InitSphereMinMaxWithVertex(const std::vector<Vertex>& _posVec);
  void UpdateSphere(const glm::vec3& point); // growing

  static const float DistanceSQ(const glm::vec3 A, const glm::vec3 B);
};

class BVolume
{
public:
  BVolume() : maxPosition(glm::vec3(0.f)), minPosition(glm::vec3(0.f)), bvType(BVT_None) {}
  BVolume(const glm::vec3 _maxPosition, const glm::vec3 _minPosition) : maxPosition(_maxPosition), minPosition(_minPosition), bvType(BVT_None) {};
  virtual ~BVolume() {};
  virtual void Draw(Shader* pShader) = 0;

  inline glm::vec3 GetMaxPos() { return maxPosition; };
  inline glm::vec3 GetMinPos() { return minPosition; };

  BVType bvType;
  glm::vec3 color = glm::vec3(1.f, 0.f, 0.f); // the default color is RED
  glm::vec3 centerPosition;

private:
  glm::vec3 maxPosition;
  glm::vec3 minPosition;
};