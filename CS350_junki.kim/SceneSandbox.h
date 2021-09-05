/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: SceneSandbox.h
Purpose: It is main scene handle almost contents
Language: C++, Microsoft C++ compiler
Platform: Visual Studio 2019, x64, Window10
Project: CS350_junki.kim
Author: junki.kim
Creation date: 5/21/2021
End Header --------------------------------------------------------*/

#ifndef SCENE_SANDBOX_H
#define SCENE_SANDBOX_H

#include "../Common/Scene.h"
#include "Camera.h"
#include "Object.h"

#include "HierarchyBottomUpAABB.h"
#include "HierarchyBottomUpSphere.h"
#include "HierarchyTopDownAABB.h"
#include "HierarchyTopDownSphere.h"

#include "AdaptiveOctree.h"
#include "BSPTree.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include <list>

enum FSQ_RenderTarget
{
  GEOMETRY = 0, // Ploygon Mode
  POSITION = 1, // Depth Buffer
  NORMALS = 2,
  ALBEDO = 3,
  SPECULAR = 4,
  LIGHTING = 5,  // Final Result

  EACH_NODE_COLOR = 6 // for Partitioning
};

enum StructureType
{
  ST_None = 0,

  // Bounding Volume Hierarchies
  BVHT_BottomUp_AABB,
  BVHT_BottomUp_Sphere,
  BVHT_TopDown_AABB,
  BVHT_TopDown_Sphere,

  // Spatial Partitionings
  SP_AdaptiveOctree,
  SP_BSP_TREE
};

class SceneSandbox : public Scene
{
public:
  SceneSandbox() = default;
  SceneSandbox(int windowWidth, int windowHeight, Camera* pCamera);
  virtual ~SceneSandbox();

  int Init() override;
  void CleanUp() override;

  int Render(const float dt) override;
  int postRender(const float dt) override;

private:
  void LoadShaders();
  void ClearShaders();

  void CreateCenterObject(std::vector<string> const& _obj_file_paths);
  void DeleteCenterObject();

  bool Init_GBuffer_Configration();
  void Render_Deferred_Objects(const GLint fbo);
  void Copy_Depth_Info(const GLint fbo, const GLint fboDefault);
  void Render_Debug_Objects();

  void RenderImGui();
  void RenderFSQ();
  void RenderBoundingVolumes();


  Camera* m_pCamera;
  std::string m_openedOBJFilePath;
  std::string m_openedFolderPath;
  std::vector<string> m_prevObjPaths;

  Shader* shaderGBuffer;
  Shader* shaderFSQBuffer;
  Shader* shaderAmbient;
  Shader* shaderNormals; // vertex and face
  Shader* shaderLines;   // bounding volumes

  std::list<Object*> pObjectList;
  std::list<Object*> pLightList;

  HierarchyBottomUpAABB* pHierBottomUpAABB;
  HierarchyBottomUpSphere* pHierBottomUpSphere;
  HierarchyTopDownAABB* pHierarchyTopDownAABB;
  HierarchyTopDownSphere* pHierarchyTopDownSphere;

  AdaptiveOctree* pAdaptiveOctree;
  BSPTree* pBSPTree;

  // G-buffer and its data
  unsigned int gBufferFBO;
  unsigned int gPosition, gNormal, gAlbedoSpec;

  // lights' info (spheres)
  int numOfSpheres;
  float spheresOrbitRadius;

  // control variables by imgui
  int renderTarget; // FSQ_RenderTarget
  bool bCopyDepthInfo;
  bool showVertexNormal;
  bool showFaceNormal;
  bool isLightObjOrbiting;
  GLfloat angleOfLightObjOrbiting;
  bool isPolygonMode;
  int boundingVolumeType; // BVType
  int structureType; // StructureType

  int maxTreeHeight = 0;
  int structureTreeLevel;
};

#endif // SCENE_SANDBOX_H
