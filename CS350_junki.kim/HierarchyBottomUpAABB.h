/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: HierarchyBottomUpAABB.h
Purpose: Header of its cpp
Language: C++, Microsoft C++ compiler
Platform: Visual Studio 2019, x64, Window10
Project: CS350_junki.kim
Author: junki.kim
Creation date: 7/24/2021
End Header --------------------------------------------------------*/

#pragma once

#include "BVolumeHierarchy.h"
#include "BoxAABB.h"

// Maximum height of tree = 7
class HierarchyBottomUpAABB : public BVolumeHierarchy
{
public:
  template <class BVolumeType>
  struct BVHList
  {
    BVHList(void) : next(nullptr) {}
    BVHList* next;
    std::list<BVolumeType*> pBVolumeList;
  };

  typedef BoxAABB VolumeType;
  typedef BVHList<VolumeType> List;

  HierarchyBottomUpAABB(std::list<Object*>& _pObjectList);
  ~HierarchyBottomUpAABB();

  void Draw(Shader* pShader);

private:
  List list;

  void ClearAllLists(List* pList);
  void BottomUpAABBAux(List* _pList, int _currentLevel);
  void Merging(std::list<VolumeType*>& _BVolumeList, List* _pNext, int _currentLevel);
  void DrawAll(Shader* pShader, List* pList);
  bool DrawLevel(Shader* pShader, List* pList, int currentLevel);
};