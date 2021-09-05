/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Object.cpp
Purpose: It contains essential attributes of an object
Language: C++, Microsoft C++ compiler
Platform: Visual Studio 2019, x64, Window10
Project: CS350_junki.kim
Author: junki.kim
Creation date: 5/21/2021
End Header --------------------------------------------------------*/

#include "Object.h"

#include "BoxAABB.h"
#include "SphereCentroidMethod.h"
#include "SphereRitterMethod.h"
#include "SphereLarssonMethod.h"
#include "SpherePCA.h"
#include "Ellipsoid.h"
#include "BoxOBB.h"

// for normal objects
Object::Object(Model* _pModel, bool needSetupMesh, glm::vec3 _pos, glm::vec3 _scale)
  : pModel(_pModel), position(_pos), scale(_scale),
  positionOffset(glm::vec3(0.f)), scaleOffset(glm::vec3(1.f)),
  scaleInitOffset(1.f)
{
  // set the default values
  color = glm::vec3(0.3f, 0.3f, 0.3f);
  isActivated = true;
  maxDist = -std::numeric_limits<float>::max();
  minDist = std::numeric_limits<float>::max();

  // calculates vertex offset for unit POSITION
  positionOffset = (_pModel->GetMaxPos() + _pModel->GetMinPos()) / 2.f;

  // calculates vertex offset for unit SCALE
  glm::vec3 numeratorUnitPos = _pModel->GetMaxPos() - positionOffset;
  glm::vec3 numerator = glm::vec3(
    (numeratorUnitPos.x ? (0.5f / numeratorUnitPos.x) : 1.f),
    (numeratorUnitPos.y ? (0.5f / numeratorUnitPos.y) : 1.f),
    (numeratorUnitPos.z ? (0.5f / numeratorUnitPos.z) : 1.f)
  );
  scaleOffset = glm::vec3(glm::min(glm::min(numerator.x, numerator.y), numerator.z));

  // re-corrdinates each vertex to unit position
    // the first vertex
  maxDistanceSQBetweenAB = 0;
  glm::vec3 diffBetweenA;
  farestVertexA = pModel->meshes.begin()->vertices.begin()->Position;
  farestVertexA -= positionOffset;
  farestVertexA *= scaleOffset;

  for (auto& mesh : pModel->meshes)
  {
    for (auto& vertex : mesh.vertices)
    {
      vertex.Position -= positionOffset;
      vertex.Position *= scaleOffset;

      // maxDist a nd minDist from the zero
      const float distFromZeroSQ =
        (vertex.Position.x * vertex.Position.x) +
        (vertex.Position.y * vertex.Position.y) +
        (vertex.Position.z * vertex.Position.z);

      maxDist = glm::max(maxDist, distFromZeroSQ);
      minDist = glm::min(minDist, distFromZeroSQ);

      // the farest vertex with A and the current vertex
      diffBetweenA = vertex.Position - farestVertexA;
      float currDistSQBetwAB =
        glm::pow(diffBetweenA.x, 2) +
        glm::pow(diffBetweenA.y, 2) +
        glm::pow(diffBetweenA.z, 2);

      if (maxDistanceSQBetweenAB < currDistSQBetwAB)
      {
        maxDistanceSQBetweenAB = currDistSQBetwAB;
        farestVertexB = vertex.Position;
      }
    }

    // now that we have all the required data, set the vertex buffers and its attribute pointers.
    if (needSetupMesh)
      mesh.setupMesh();

    maxDist = glm::sqrt(maxDist);
    minDist = glm::sqrt(minDist);
  }
}

// for "Powar Plant" type of objects (scaleInitOffset)
Object::Object(Model* _pModel, float _scaleInitOffset, bool needSetupMesh, glm::vec3 _pos, glm::vec3 _scale)
  : pModel(_pModel), position(_pos), scale(_scale),
  positionOffset(glm::vec3(0.f)), scaleOffset(glm::vec3(1.f)),
  scaleInitOffset(_scaleInitOffset)
{
  const glm::vec3 centerPos = (GetMaxPos() + GetMinPos()) / 2.f;

  // set the default values
  color = glm::vec3(0.3f, 0.3f, 0.3f);
  isActivated = true;
  maxDist = -std::numeric_limits<float>::max();
  minDist = std::numeric_limits<float>::max();

  // re-corrdinates each vertex to unit position
    // the first vertex
  maxDistanceSQBetweenAB = 0;
  glm::vec3 diffBetweenA;
  farestVertexA = pModel->meshes.begin()->vertices.begin()->Position;
  farestVertexA *= _scaleInitOffset;

  for (auto& mesh : pModel->meshes)
  {
    for (auto& vertex : mesh.vertices)
    {
      vertex.Position *= _scaleInitOffset;

      // maxDist and minDist from the center position
      const float distFromCenterSQ =
        glm::pow(vertex.Position.x - centerPos.x, 2) +
        glm::pow(vertex.Position.y - centerPos.y, 2) +
        glm::pow(vertex.Position.z - centerPos.z, 2);

      maxDist = glm::max(maxDist, distFromCenterSQ);
      minDist = glm::min(minDist, distFromCenterSQ);

      // the farest vertex with A and the current vertex
      diffBetweenA = vertex.Position - farestVertexA;
      float currDistSQBetwAB =
        (diffBetweenA.x * diffBetweenA.x) +
        (diffBetweenA.y * diffBetweenA.y) +
        (diffBetweenA.z * diffBetweenA.z);

      if (maxDistanceSQBetweenAB < currDistSQBetwAB)
      {
        maxDistanceSQBetweenAB = currDistSQBetwAB;
        farestVertexB = vertex.Position;
      }
    }

    // now that we have all the required data, set the vertex buffers and its attribute pointers.
    if (needSetupMesh)
      mesh.setupMesh();

    // may cause a bug in the future
    maxDist = glm::sqrt(maxDist);
    minDist = glm::sqrt(minDist);
  }
}

Object::~Object()
{
  // Each type of Bounding Volumes
  for (auto pObj : pBVolumeList)
  {
    delete pObj;
    pObj = nullptr;
  }
  pBVolumeList.clear();

  // Model
  delete pModel;
  pModel = nullptr;
}

void Object::SetupMesh()
{
  for (auto& mesh : pModel->meshes)
  {
    // now that we have all the required data, set the vertex buffers and its attribute pointers.
    mesh.setupMesh();
  }
}

void Object::Draw(Shader* pShader)
{
  if (isActivated)
    pModel->Draw(pShader);
}

// Bounding Volumes
void Object::AddBoundingVolumes()
{
  AddBoxAABB();
  AddSphereCentroidMethod();
  AddSphereRitterMethod();
  AddSphereLarssonMethod();
  AddSpherePCA();
  AddEllipsoid();
  AddBoxOBB();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void Object::AddBoxAABB()
{
  BoxAABB* pBVol = new BoxAABB(GetMaxPos(), GetMinPos());

  pBVolumeList.push_back(dynamic_cast<BVolume*>(pBVol));
}

void Object::AddSphereCentroidMethod()
{
  SphereCentroidMethod* pBVol = new SphereCentroidMethod(GetMaxPos(), GetMinPos(), maxDist);

  pBVolumeList.push_back(dynamic_cast<BVolume*>(pBVol));
}

void Object::AddSphereRitterMethod()
{
  SphereRitterMethod* pBVol = new SphereRitterMethod(GetMaxPos(), GetMinPos(), this);

  pBVolumeList.push_back(dynamic_cast<BVolume*>(pBVol));
}

void Object::AddSphereLarssonMethod()
{
  SphereLarssonMethod* pBVol = new SphereLarssonMethod(GetMaxPos(), GetMinPos(), this);

  pBVolumeList.push_back(dynamic_cast<BVolume*>(pBVol));
}

void Object::AddSpherePCA()
{
  SpherePCA* pBVol = new SpherePCA(GetMaxPos(), GetMinPos(), this);

  pBVolumeList.push_back(dynamic_cast<BVolume*>(pBVol));
}

void Object::AddEllipsoid()
{
  Ellipsoid* pBVol = new Ellipsoid(GetMaxPos(), GetMinPos(), this);

  pBVolumeList.push_back(dynamic_cast<BVolume*>(pBVol));
}

void Object::AddBoxOBB()
{
  BoxOBB* pBVol = new BoxOBB(GetMaxPos(), GetMinPos(), this);

  pBVolumeList.push_back(dynamic_cast<BVolume*>(pBVol));
}