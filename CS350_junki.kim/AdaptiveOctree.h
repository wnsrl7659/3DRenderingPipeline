/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: AdaptiveOctree.h
Purpose: It's a header file for AdaptiveOctree.cpp
Language: C++, Microsoft C++ compiler
Platform: Visual Studio 2019, x64, Window10
Project: CS350_junki.kim
Author: junki.kim
Creation date: 7/24/2021
End Header --------------------------------------------------------*/

#pragma once

#include "Object.h"
#include "BoxAABB.h"

#include "../Common/Shader.hpp"

class AdaptiveOctree
{
public:
  AdaptiveOctree(std::list<Object*>& _pObjectList);
  ~AdaptiveOctree();

  void Draw(Shader* pShader);
  const int GetMaxDepth() { return m_MaxDepth; }

  int targetTreeHeight;

private:
  struct Octree {
    glm::vec3 center;         // Center point of octree node (not strictly needed)
    float halfWidth;          // Half the width of the node volume (not strictly needed)
    Octree* pChild = nullptr; // Pointers to the eight children nodes
    BoxAABB* pBox;            // Box to represent each volume
    int currDepth;
  };

  void ClearAllNodes(Octree* pOctree); // Post order
  void BuildChildNodes(Octree* pTree, const glm::vec3 center, const float halfWidth, const int currDepth);
  void InsertVertices(Octree* pTree, vector<Vertex*> _pPointCloud);

  void DrawAux(Shader* pShader, Octree* pNode, int currentLevel); // Post order

  Octree* m_pOctree;
  int m_MaxDepth;
};