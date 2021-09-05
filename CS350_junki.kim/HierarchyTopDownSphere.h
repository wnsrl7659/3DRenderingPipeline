/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: HierarchyTopDownSphere.h
Purpose: It proceed by partitioning the input set into two (or more)
         subsets, bounding them in the chosen bounding volume, then keep
         partitioning (and bounding) recursively until each subset consists
         of only a single primitive (leaf nodes are reached).
Language: C++, Microsoft C++ compiler
Platform: Visual Studio 2019, x64, Window10
Project: CS350_junki.kim
Author: junki.kim
Creation date: 7/7/2021
End Header --------------------------------------------------------*/

#pragma once

#include "Object.h"
#include "BVolumeHierarchy.h"
#include "SphereCentroidMethod.h"

#include <list>

// Maximum height of tree = 7
class HierarchyTopDownSphere : public BVolumeHierarchy
{
public:
  template <class BVolumeType>
  struct BVHNode
  {
    BVHNode(void) : left(nullptr), right(nullptr), pBVolume(nullptr) {}
    BVHNode* left;
    BVHNode* right;
    BVolumeType* pBVolume;
  };

  typedef SphereCentroidMethod VolumeType;
  typedef BVHNode<VolumeType> Node;

  HierarchyTopDownSphere(std::list<Object*>& _pObjectList)
  {
    int currentLevel = 0;

    std::vector<glm::vec3> pointCloud;

    for (Object* pObj : _pObjectList)
    {
      for (auto mesh : pObj->pModel->meshes)
      {
        for (auto vertex : mesh.vertices)
          pointCloud.push_back(vertex.Position);
      }
    }

    TopDownSphereAux(pointCloud, &(this->node), currentLevel);
  }

  ~HierarchyTopDownSphere()
  {
    ClearAllNodes(&(this->node));
  }

  void TopDownSphereAux(const std::vector<glm::vec3>& pointCloud, Node* pNode, int _currentLevel)
  {
    // end when it reaches maximum height
    if (_currentLevel > MAX_BVTREE_HEIGHT || pointCloud.size() == 0)
      return;

    const float colorVal = _currentLevel + 1;
    const glm::vec3 currLevelColor = glm::vec3(
      1.f - (colorVal * colorVal / float(MAX_BVTREE_HEIGHT * MAX_BVTREE_HEIGHT)),
      colorVal / float(MAX_BVTREE_HEIGHT),
      colorVal * colorVal / float(MAX_BVTREE_HEIGHT * MAX_BVTREE_HEIGHT)
    );

    //////////////////////////////////////////
    // Ritter's method

    BVSphere bVolumesphere;
    bVolumesphere.InitSphereMinMaxWithVec3(pointCloud);
    for (const auto& point : pointCloud)
      bVolumesphere.UpdateSphere(point);

    //////////////////////////////////////////

    VolumeType* pBVol = new VolumeType(bVolumesphere.center, bVolumesphere.radius);
    pBVol->color = currLevelColor;
    pNode->pBVolume = pBVol;

    Node* pLeft = new Node();
    pNode->left = pLeft;

    Node* pRight = new Node();
    pNode->right = pRight;

    std::vector<glm::vec3> pointCloudLeft;
    std::vector<glm::vec3> pointCloudRight;

    Partitioning(pointCloud, pointCloudLeft, pointCloudRight);

    //if (pointCloudLeft.size() >= MAX_HARD_CUTOFF_VERTICES)
    TopDownSphereAux(pointCloudLeft, pLeft, _currentLevel + 1);

    //if (pointCloudRight.size() >= MAX_HARD_CUTOFF_VERTICES)
    TopDownSphereAux(pointCloudRight, pRight, _currentLevel + 1);

  }

  void Partitioning(const std::vector<glm::vec3>& _pointCloud, std::vector<glm::vec3>& _pointCloudLeft, std::vector<glm::vec3>& _pointCloudRight)
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
    for (const glm::vec3& point : _pointCloud)
    {
      maxAxis.x = max(maxAxis.x, point.x);
      maxAxis.y = max(maxAxis.y, point.y);
      maxAxis.z = max(maxAxis.z, point.z);

      minAxis.x = min(minAxis.x, point.x);
      minAxis.y = min(minAxis.y, point.y);
      minAxis.z = min(minAxis.z, point.z);
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
    for (const glm::vec3& point : _pointCloud)
      center += point;
    center /= float(_pointCloud.size());

    // distribute to the left and right nodes
    for (const glm::vec3& point : _pointCloud)
    {
      glm::vec3 toPoint = point - center;
      if (glm::dot(toPoint, splittingTargetAxis) > 0)
        _pointCloudLeft.push_back(point);
      else
        _pointCloudRight.push_back(point);
    }
  }

  // Post order
  void ClearAllNodes(Node* pNode)
  {
    if (pNode == NULL)
      return;

    ClearAllNodes(pNode->left);  // Visit left subtree
    ClearAllNodes(pNode->right); // Visit right subtree

    delete pNode->pBVolume;
    pNode->pBVolume = nullptr;

    if (pNode->left != nullptr)
    {
      delete pNode->left;
      pNode->left = nullptr;
    }

    if (pNode->right != nullptr)
    {
      delete pNode->right;
      pNode->right = nullptr;
    }
  }

  void Draw(Shader* pShader)
  {
    DrawAux(pShader, &(this->node), 0);
  }

  // Post order
  void DrawAux(Shader* pShader, Node* pNode, int currentLevel)
  {
    if (pNode == NULL || pNode->pBVolume == NULL)
      return;

    if (targetTreeHeight != -1 && currentLevel > targetTreeHeight)
      return;

    DrawAux(pShader, pNode->left, currentLevel + 1);  // Visit left subtree
    DrawAux(pShader, pNode->right, currentLevel + 1); // Visit right subtree

    if (currentLevel == targetTreeHeight || targetTreeHeight == -1)
      pNode->pBVolume->Draw(pShader);
  }

private:
  Node node;
};