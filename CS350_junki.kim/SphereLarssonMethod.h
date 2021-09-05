/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: SphereLarssonMethod.h
Purpose: It's the header file of SphereLarssonMethod
Language: C++, Microsoft C++ compiler
Platform: Visual Studio 2019, x64, Window10
Project: CS350_junki.kim
Author: junki.kim
Creation date: 7/7/2021
End Header --------------------------------------------------------*/
#pragma once

#include "BVolume.h"
#include "Object.h"

class SphereLarssonMethod : public BVolume
{
public:
  SphereLarssonMethod(const glm::vec3 _maxPosition, const glm::vec3 _minPosition, Object* pObj);
  ~SphereLarssonMethod();

  void Draw(Shader* pShader);

private:
  Object* pSphere;
};