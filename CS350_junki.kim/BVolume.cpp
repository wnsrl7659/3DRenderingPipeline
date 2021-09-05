/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: BVolume.cpp
Purpose: This is a base class for all types of bounding volumes.
Language: C++, Microsoft C++ compiler
Platform: Visual Studio 2019, x64, Window10
Project: CS350_junki.kim
Author: junki.kim
Creation date: 7/7/2021
End Header --------------------------------------------------------*/

#pragma once

#include "BVolume.h"

void BVSphere::InitSphereMinMaxWithVec3(const std::vector<glm::vec3>& _posVec)
{
  unsigned maxIndex = 0;
  unsigned minIndex = 0;

  unsigned maxX = 0;
  unsigned maxY = 0;
  unsigned maxZ = 0;

  unsigned minX = 0;
  unsigned minY = 0;
  unsigned minZ = 0;

  maxAxis = glm::vec3(-FLT_MAX);
  minAxis = glm::vec3(FLT_MAX);

  for (unsigned i = 0; i < _posVec.size(); ++i)
  {
    const glm::vec3& vertex = _posVec[i];

    // max vertex
    if (vertex.x > _posVec[maxX].x)
      minX = i;
    if (vertex.y > _posVec[maxY].y)
      minY = i;
    if (vertex.z > _posVec[maxZ].z)
      minZ = i;

    // min vertex
    if (vertex.x < _posVec[minX].x)
      minX = i;
    if (vertex.y < _posVec[minY].y)
      minY = i;
    if (vertex.z < _posVec[minZ].z)
      minZ = i;

    // max axis
    maxAxis.x = glm::max(maxAxis.x, vertex.x);
    maxAxis.y = glm::max(maxAxis.y, vertex.y);
    maxAxis.z = glm::max(maxAxis.z, vertex.z);

    // min axis
    minAxis.x = glm::min(minAxis.x, vertex.x);
    minAxis.y = glm::min(minAxis.y, vertex.y);
    minAxis.z = glm::min(minAxis.z, vertex.z);
  }

  // distance * distance
  float distXSQ = DistanceSQ(_posVec[minX], _posVec[maxX]);
  float distYSQ = DistanceSQ(_posVec[minY], _posVec[maxY]);
  float distZSQ = DistanceSQ(_posVec[minZ], _posVec[maxZ]);

  minIndex = minX;
  maxIndex = maxX;

  if (distYSQ > distXSQ && distYSQ > distZSQ)
  {
    minIndex = minY;
    maxIndex = maxY;
  }
  else if (distZSQ > distXSQ && distZSQ > distYSQ)
  {
    minIndex = minZ;
    maxIndex = maxZ;
  }

  const glm::vec3 min = _posVec[minIndex];
  const glm::vec3 max = _posVec[maxIndex];

  // init or update
  center = (min + max) / 2.f;
  radius = glm::distance(center, max);
  maxVertex = max;
  minVertex = min;
}

void BVSphere::InitSphereMinMaxWithVertex(const std::vector<Vertex>& _posVec)
{
  unsigned maxIndex = 0;
  unsigned minIndex = 0;

  unsigned maxX = 0;
  unsigned maxY = 0;
  unsigned maxZ = 0;

  unsigned minX = 0;
  unsigned minY = 0;
  unsigned minZ = 0;

  maxAxis = glm::vec3(-FLT_MAX);
  minAxis = glm::vec3(FLT_MAX);

  for (unsigned i = 0; i < _posVec.size(); ++i)
  {
    const glm::vec3& vertex = _posVec[i].Position;

    // max vertex
    if (vertex.x > _posVec[maxX].Position.x)
      minX = i;
    if (vertex.y > _posVec[maxY].Position.y)
      minY = i;
    if (vertex.z > _posVec[maxZ].Position.z)
      minZ = i;

    // min vertex
    if (vertex.x < _posVec[minX].Position.x)
      minX = i;
    if (vertex.y < _posVec[minY].Position.y)
      minY = i;
    if (vertex.z < _posVec[minZ].Position.z)
      minZ = i;

    // max axis
    maxAxis.x = glm::max(maxAxis.x, vertex.x);
    maxAxis.y = glm::max(maxAxis.y, vertex.y);
    maxAxis.z = glm::max(maxAxis.z, vertex.z);

    // min axis
    minAxis.x = glm::min(minAxis.x, vertex.x);
    minAxis.y = glm::min(minAxis.y, vertex.y);
    minAxis.z = glm::min(minAxis.z, vertex.z);
  }

  // distance * distance
  float distXSQ = DistanceSQ(_posVec[minX].Position, _posVec[maxX].Position);
  float distYSQ = DistanceSQ(_posVec[minY].Position, _posVec[maxY].Position);
  float distZSQ = DistanceSQ(_posVec[minZ].Position, _posVec[maxZ].Position);

  minIndex = minX;
  maxIndex = maxX;

  if (distYSQ > distXSQ && distYSQ > distZSQ)
  {
    minIndex = minY;
    maxIndex = maxY;
  }
  else if (distZSQ > distXSQ && distZSQ > distYSQ)
  {
    minIndex = minZ;
    maxIndex = maxZ;
  }

  const glm::vec3 min = _posVec[minIndex].Position;
  const glm::vec3 max = _posVec[maxIndex].Position;

  // init or update
  center = (min + max) / 2.f;
  radius = glm::distance(center, max);
  maxVertex = max;
  minVertex = min;
}

void BVSphere::UpdateSphere(const glm::vec3& point)
{
  glm::vec3 toPoint = point - center;
  float distSQ = DistanceSQ(toPoint, glm::vec3(0.f));

  // check other points outside
  if (distSQ > (radius * radius))
  {
    float dist = sqrt(distSQ);
    float newRadius = (radius + dist) / 2.f;
    float adjustment = (newRadius - radius) / dist;

    // update
    radius = newRadius;
    center += toPoint * adjustment;
  }
}

const float BVSphere::DistanceSQ(const glm::vec3 A, const glm::vec3 B)
{
  glm::vec3 diff = B - A;
  return float((diff.x * diff.x) + (diff.y * diff.y) + (diff.z * diff.z));
}