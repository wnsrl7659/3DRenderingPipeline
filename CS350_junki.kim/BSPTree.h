/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: BSPTree.h
Purpose: It's a header file for BSPTree.cpp
Language: C++, Microsoft C++ compiler
Platform: Visual Studio 2019, x64, Window10
Project: CS350_junki.kim
Author: junki.kim
Creation date: 7/24/2021
End Header --------------------------------------------------------*/

#pragma once

#include "Object.h"

#include "../Common/Shader.hpp"

class BSPTree
{
public:
  BSPTree(std::list<Object*>& _pObjectList);
  ~BSPTree();

private:
  void TopDownAux(vector<Vertex*>& pointCloud);
  void Partitioning(std::vector<Vertex*>& _pointCloud, std::vector<Vertex*>& _pointCloudLeft, std::vector<Vertex*>& _pointCloudRight);
};