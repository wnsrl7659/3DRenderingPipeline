/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: BVolumeHierarchy.h
Purpose: create a bounding volume for every individual object.
         and use the nearest	neighbor to combine bounding volumes at
         child-level into a parent node.
Language: C++, Microsoft C++ compiler
Platform: Visual Studio 2019, x64, Window10
Project: CS350_junki.kim
Author: junki.kim
Creation date: 7/7/2021
End Header --------------------------------------------------------*/

#pragma once

#include "Object.h"
#include "BVolume.h"

#include <list>

#define MAX_BVTREE_HEIGHT 7
#define MAX_HARD_CUTOFF_VERTICES 500

class BVolumeHierarchy
{
public:
  BVolumeHierarchy() : targetTreeHeight(0) {}
  virtual void Draw(Shader* pShader) = 0;

  int targetTreeHeight;
};