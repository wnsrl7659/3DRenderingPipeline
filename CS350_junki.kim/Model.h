/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Model.h
Purpose: It's a header file for model.cpp ref from LearnOpenGL
Language: C++, Microsoft C++ compiler
Platform: Visual Studio 2019, x64, Window10
Project: CS350_junki.kim
Author: junki.kim
Creation date: 5/21/2021
End Header --------------------------------------------------------*/

#ifndef MODEL_H
#define MODEL_H

#include "Mesh.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
using namespace std;

unsigned int TextureFromFile(const char* path, const string& directory, bool gamma = false);

class Model
{
public:
  vector<Texture> textures_loaded;
  vector<Mesh> meshes;
  string directory;
  bool gammaCorrection;

  Model(string const& path, bool gamma = false);

  void Draw(Shader* pShader);

  inline glm::vec3 GetMaxPos() { return maxPosition; }
  inline glm::vec3 GetMinPos() { return minPosition; }

private:
  // loads a model with ASSIMP extensions
  void loadModel(string const& path);

  void processNode(aiNode* node, const aiScene* scene);
  Mesh processMesh(aiMesh* mesh, const aiScene* scene);

  // checks all material textures of a given type and loads the textures
  vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName);

  glm::vec3 maxPosition;
  glm::vec3 minPosition;
};

#endif