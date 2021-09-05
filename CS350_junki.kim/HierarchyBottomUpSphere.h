/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: HierarchyBottomUpSphere.h
Purpose: Header of HierarchyBottomUpSphere.cpp
Language: C++, Microsoft C++ compiler
Platform: Visual Studio 2019, x64, Window10
Project: CS350_junki.kim
Author: junki.kim
Creation date: 7/24/2021
End Header --------------------------------------------------------*/

#pragma once

#include "BVolumeHierarchy.h"
#include "SphereCentroidMethod.h"

// Maximum height of tree = 7
class HierarchyBottomUpSphere : public BVolumeHierarchy
{
public:
  template <class BVolumeType>
  struct BVHList
  {
    BVHList(void) : next(nullptr) {}
    BVHList* next;
    std::list<BVolumeType*> pBVolumeList;
  };

  typedef SphereCentroidMethod VolumeType;
  typedef BVHList<VolumeType> List;

  HierarchyBottomUpSphere(std::list<Object*>& _pObjectList)
  {
    for (Object* pObj : _pObjectList)
    {
      VolumeType* pBVol = new VolumeType(pObj->GetMaxPos(), pObj->GetMinPos(), pObj->GetMaxDist());
      pBVol->color = glm::vec3(1.f, 0.f, 0.f); // red color for the first hight level

      list.pBVolumeList.push_back(pBVol);
    }

    BottomUpSphereAux(&(this->list), 1);
  }

  ~HierarchyBottomUpSphere()
  {
    ClearAllLists(&(this->list));
  }

  void Draw(Shader* pShader)
  {
    if (targetTreeHeight == -1)
      DrawAll(pShader, &(this->list));
    else
      DrawLevel(pShader, &(this->list), 0);
  }

private:
  List list;

  void ClearAllLists(List* pList)
  {
    if (pList == NULL)
      return;

    ClearAllLists(pList->next);

    for (auto pBox : pList->pBVolumeList)
    {
      delete pBox;
      pBox = nullptr;
    }

    pList->pBVolumeList.clear();
  }

  void BottomUpSphereAux(List* _pList, int _currentLevel)
  {
    // reachs to the maximum height
    if (_currentLevel > MAX_BVTREE_HEIGHT)
      return;

    // there are no given objects.
    if (_pList->pBVolumeList.size() == 0)
      return;

    ///////////////////////////////////////////////////////////////////////////

    List* pNext = new List();
    _pList->next = pNext;

    Merging(_pList->pBVolumeList, _pList->next, _currentLevel);

    // delete if there is no bouding volume
    if (_pList->next->pBVolumeList.size() == 0)
    {
      delete _pList->next;
      _pList->next = nullptr;
    }

    ///////////////////////////////////////////////////////////////////////////

    BottomUpSphereAux(pNext, _currentLevel + 1);
  }

  void Merging(std::list<VolumeType*>& _BVolumeList, List* _pNext, int _currentLevel)
  {
    const int numOfVolumes = _BVolumeList.size();
    const glm::vec3 currLevelColor = glm::vec3(
      1.f - (_currentLevel * _currentLevel / float(MAX_BVTREE_HEIGHT * MAX_BVTREE_HEIGHT)),
      _currentLevel / float(MAX_BVTREE_HEIGHT),
      _currentLevel * _currentLevel / float(MAX_BVTREE_HEIGHT * MAX_BVTREE_HEIGHT)
    );

    // one or no object, no needs to partition
    if (numOfVolumes < 2)
      return;

    // more than one object, needs to calculate
    std::list<VolumeType*> tempList = _BVolumeList;
    while (tempList.size() > 1)
    {
      VolumeType* pObjA = *(tempList.begin());
      VolumeType* pObjB = nullptr;
      glm::vec3 rightMostPos;
      glm::vec3 leftMostPos;

      float distSQMin = FLT_MAX;
      // find the closest object with the Obj A
      for (std::list<VolumeType*>::iterator iter = ++(tempList.begin()); iter != tempList.end(); ++iter)
      {
        glm::vec3 rightMostPosTemp;
        glm::vec3 leftMostPosTemp;

        rightMostPosTemp.x = max(pObjA->GetMaxPos().x, (*iter)->GetMaxPos().x);
        rightMostPosTemp.y = max(pObjA->GetMaxPos().y, (*iter)->GetMaxPos().y);
        rightMostPosTemp.z = max(pObjA->GetMaxPos().z, (*iter)->GetMaxPos().z);

        leftMostPosTemp.x = min(pObjA->GetMinPos().x, (*iter)->GetMinPos().x);
        leftMostPosTemp.y = min(pObjA->GetMinPos().y, (*iter)->GetMinPos().y);
        leftMostPosTemp.z = min(pObjA->GetMinPos().z, (*iter)->GetMinPos().z);

        glm::vec3 diff = rightMostPosTemp - leftMostPosTemp;
        const float distSQ = (diff.x * diff.x) + (diff.y * diff.y) + (diff.z * diff.z);

        if (distSQMin > distSQ)
        {
          pObjB = *iter;
          distSQMin = distSQ;
          rightMostPos = rightMostPosTemp;
          leftMostPos = leftMostPosTemp;
        }
      }

      // calculate the radius of the merged sphere.
      //glm::vec3 centerPos = (rightMostPos + leftMostPos) / 2.f;
      glm::vec3 diff = leftMostPos - rightMostPos;
      const float distSQ = (diff.x * diff.x) + (diff.y * diff.y) + (diff.z * diff.z);
      float radius = (glm::sqrt(distSQ) / 2.f);

      VolumeType* pBVol = new VolumeType(rightMostPos, leftMostPos, radius);
      pBVol->color = currLevelColor; // red color for the first hight level
      _pNext->pBVolumeList.push_back(pBVol);

      tempList.remove(pObjA);
      tempList.remove(pObjB);
    }

    // In case of an odd number of objects
    if (tempList.size() != 0)
    {
      VolumeType* pBVol = new VolumeType(
        (*tempList.begin())->GetMaxPos(),
        (*tempList.begin())->GetMinPos(),
        (*tempList.begin())->GetRadius()
      );
      pBVol->color = currLevelColor;
      _pNext->pBVolumeList.push_back(pBVol);
    }
  }

  void DrawAll(Shader* pShader, List* pList)
  {
    if (pList == NULL)
      return;

    DrawAll(pShader, pList->next);

    for (VolumeType* pBVolume : pList->pBVolumeList)
      pBVolume->Draw(pShader);
  }

  bool DrawLevel(Shader* pShader, List* pList, int currentLevel)
  {
    if (pList == NULL || currentLevel == targetTreeHeight + 1)
      return true;

    if (DrawLevel(pShader, pList->next, currentLevel + 1))
    {
      for (VolumeType* pBVolume : pList->pBVolumeList)
        pBVolume->Draw(pShader);
    }

    return false;
  }
};