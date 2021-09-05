/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: BSPTree.cpp
Purpose: Use top-down criteria for BSP tree creation. Construct the BSP tree
         using the following approaches:
           1) Leaf storing. This means that every leaf node contains geometry,
              but internal nodes only contain information about the split plane.
           2) Node Storing. A variant on leaf-storing trees. Store the coplanar
              polygons with the split plane in the internal node of the tree.
Language: C++, Microsoft C++ compiler
Platform: Visual Studio 2019, x64, Window10
Project: CS350_junki.kim
Author: junki.kim
Creation date: 7/24/2021
End Header --------------------------------------------------------*/

#include "BSPTree.h"

BSPTree::BSPTree(std::list<Object*>& _pObjectList)
{
  vector<Vertex*> pointCloud;

  // Find max and min positions to init the first entire volume
  glm::vec3 maxPosition = glm::vec3(-std::numeric_limits<float>::max());
  glm::vec3 minPosition = glm::vec3(std::numeric_limits<float>::max());
  for (Object* pObj : _pObjectList)
  {
    maxPosition = glm::max(maxPosition, pObj->GetMaxPos());
    minPosition = glm::min(minPosition, pObj->GetMinPos());

    for (auto& mesh : pObj->pModel->meshes)
    {
      for (auto& vertex : mesh.vertices)
        pointCloud.push_back(&vertex);
    }
  }

  TopDownAux(pointCloud);
}

BSPTree::~BSPTree()
{

}

void BSPTree::TopDownAux(vector<Vertex*>& pointCloud)
{
  // Terminating criteria should be 300 (or less) triangles in the current cell.
  if (pointCloud.size() < 300)
    return;

  std::vector<Vertex*> pointCloudLeft;
  std::vector<Vertex*> pointCloudRight;

  Partitioning(pointCloud, pointCloudLeft, pointCloudRight);

  TopDownAux(pointCloudLeft);
  TopDownAux(pointCloudRight);

}

void BSPTree::Partitioning(std::vector<Vertex*>& _pointCloud, std::vector<Vertex*>& _pointCloudLeft, std::vector<Vertex*>& _pointCloudRight)
{
  if (_pointCloud.size() == 0)
    return;

  // init
  glm::vec3 proj(0.f);
  const glm::vec3 xAxis(1.0f, 0.0f, 0.0f);
  const glm::vec3 yAxis(0.0f, 1.0f, 0.0f);
  const glm::vec3 zAxis(0.0f, 0.0f, 1.0f);

  // find max and min axis
  glm::vec3 maxAxis(-FLT_MAX);
  glm::vec3 minAxis(FLT_MAX);
  for (auto& vertex : _pointCloud)
  {
    maxAxis.x = max(maxAxis.x, vertex->Position.x);
    maxAxis.y = max(maxAxis.y, vertex->Position.y);
    maxAxis.z = max(maxAxis.z, vertex->Position.z);

    minAxis.x = min(minAxis.x, vertex->Position.x);
    minAxis.y = min(minAxis.y, vertex->Position.y);
    minAxis.z = min(minAxis.z, vertex->Position.z);
  }

  glm::vec3 diffSQ = maxAxis - minAxis;
  diffSQ = diffSQ * diffSQ;

  // find the spreadest axis
  glm::vec3 splittingTargetAxis;
  if (diffSQ.y > diffSQ.x && diffSQ.y > diffSQ.z)      // y
    splittingTargetAxis = yAxis;
  else if (diffSQ.z > diffSQ.x && diffSQ.z > diffSQ.y) // z
    splittingTargetAxis = zAxis;
  else                                                 // x
    splittingTargetAxis = xAxis;

  // find the mid point
  glm::vec3 center(0.f);
  for (auto& vertex : _pointCloud)
    center += vertex->Position;
  center /= float(_pointCloud.size());

  // Display each level in different color
  glm::vec3 color;
  // random color between 0.4 and 1.0
  color.x = ((rand() % 100) / 200.0f) + 0.4f;
  color.y = ((rand() % 100) / 200.0f) + 0.4f;
  color.z = ((rand() % 100) / 200.0f) + 0.4f;

  // distribute to the left and right nodes
  for (auto& vertex : _pointCloud)
  {
    glm::vec3 toPoint = vertex->Position - center;
    if (glm::dot(toPoint, splittingTargetAxis) > 0)
    {
      vertex->NodeColor = glm::vec3(color);
      _pointCloudLeft.push_back(vertex);
    }
    else
    {
      vertex->NodeColor = glm::vec3(color.z, color.x, color.y); // just a different color
      _pointCloudRight.push_back(vertex);
    }
  }
}