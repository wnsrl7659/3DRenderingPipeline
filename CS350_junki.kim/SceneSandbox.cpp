/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: SceneSandbox.cpp
Purpose: It is main scene handle almost contents
Language: C++, Microsoft C++ compiler
Platform: Visual Studio 2019, x64, Window10
Project: CS350_junki.kim
Author: junki.kim
Creation date: 5/21/2021
End Header --------------------------------------------------------*/

#include "SceneSandbox.h"

// glm for OpenGL data types
#include <glm/vec3.hpp>
#include <glm/detail/type_mat.hpp>

// imgui for making UI
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h" // PushItemFlag

#include <filesystem> // load files in a folder

#define MAX_LIGHT_NUMBER 16
#define DEBUG_GEOMETRY_COLOR glm::vec3(0.f, 1.f, 0.f)

SceneSandbox::~SceneSandbox()
{
  this->CleanUp();
}

SceneSandbox::SceneSandbox(int windowWidth, int windowHeight, Camera* pCamera)
  : Scene(windowWidth, windowHeight), m_pCamera(pCamera)
{
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "bugprone-unused-return-value"
int SceneSandbox::Init()
{
  // Init the camera
  m_pCamera->Position = glm::vec3(0.0f, 2.5f, 5.65f);
  m_pCamera->Pitch = -20.f;
  m_pCamera->updateCameraVectors();

  // control variables by imgui
  s_pPFDOpenFile = nullptr;
  m_openedOBJFilePath = "../Common/models/bunny.obj";
  s_pPFDOpenFolder = nullptr;
  m_openedFolderPath = "../Common/models";

  numOfSpheres = 8;
  spheresOrbitRadius = 2.f;

  renderTarget = FSQ_RenderTarget::EACH_NODE_COLOR;
  bCopyDepthInfo = true;
  showVertexNormal = false;
  showFaceNormal = false;
  boundingVolumeType = BVT_None;
  structureType = SP_AdaptiveOctree;
  structureTreeLevel = 0;

  isLightObjOrbiting = true;
  angleOfLightObjOrbiting = 0.f;

  isPolygonMode = false;

  // configure global opengl state
  glEnable(GL_DEPTH_TEST);

  // build and compile shaders
  SceneSandbox::LoadShaders();

  // load models and set attributes
  {
    // CENTER OBJECT
    CreateCenterObject(std::vector<std::string>(1, m_openedOBJFilePath));

    // LIGHTS (SPHERES) around the center object
    for (int i = 0; i < MAX_LIGHT_NUMBER; i++)
    {
      // model
      Object* pLight = new Object(new Model("../Common/models/sphere.obj"));

      // position
      float theta = 2.0f * glm::pi<float>() * float(i) / float(numOfSpheres); //get the current angle
      float x = spheresOrbitRadius * cosf(theta); //calculate the x component
      float z = spheresOrbitRadius * sinf(theta); //calculate the y component
      pLight->position = glm::vec3(x, 0.f, z);

      // scale
      pLight->scale = glm::vec3(0.25f);

      // random color between 0.6 and 1.0
      float rColor = ((rand() % 100) / 200.0f) + 0.6;
      float gColor = ((rand() % 100) / 200.0f) + 0.6;
      float bColor = ((rand() % 100) / 200.0f) + 0.6;
      pLight->color = glm::vec3(rColor, gColor, bColor);

      if (i >= numOfSpheres)
        pLight->isActivated = false;

      pLightList.push_back(pLight);
    }
  }

  // configure g-buffer framebuffer
  Init_GBuffer_Configration();

  return Scene::Init();
}
#pragma clang diagnostic pop

void SceneSandbox::CleanUp()
{
  for (auto pObj : pLightList)
  {
    delete pObj;
    pObj = nullptr;
  }
  pLightList.clear();

  DeleteCenterObject();

  // shaders
  ClearShaders();
}

int SceneSandbox::Render(const float dt)
{
  if (isPolygonMode || (renderTarget == EACH_NODE_COLOR))
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  else if (!isPolygonMode || (renderTarget != EACH_NODE_COLOR))
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  // ImGUI
  RenderImGui();

  // render config
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // “gBufferFBO?is user defined FBO
  Render_Deferred_Objects(gBufferFBO);

  // Copy gBufferFBO’s depth buffer into default OpenGL depth buffer (0)
  if (bCopyDepthInfo) // Toggle this capability with input from user (GUI).
    Copy_Depth_Info(gBufferFBO, 0);

  // This will draw all dubug data.
  Render_Debug_Objects();

  return 0;
}

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
int SceneSandbox::postRender(const float dt)
{
  if (isLightObjOrbiting)
  {
    angleOfLightObjOrbiting += (dt * 0.5f); // orbiting speed
    if (angleOfLightObjOrbiting >= glm::two_pi<float>())
      angleOfLightObjOrbiting = 0.f;
  }

  return 0;
}

bool SceneSandbox::Init_GBuffer_Configration()
{
  glGenFramebuffers(1, &gBufferFBO);
  glBindFramebuffer(GL_FRAMEBUFFER, gBufferFBO);

  // position buffer (GL_RGB)
  glGenTextures(1, &gPosition);
  glBindTexture(GL_TEXTURE_2D, gPosition);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, this->_windowWidth, this->_windowHeight, 0, GL_RGB, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

  // normal buffer (GL_RGB)
  glGenTextures(1, &gNormal);
  glBindTexture(GL_TEXTURE_2D, gNormal);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, this->_windowWidth, this->_windowHeight, 0, GL_RGB, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

  // color (albedo) and specular buffer (GL_RGBA)
  glGenTextures(1, &gAlbedoSpec);
  glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, this->_windowWidth, this->_windowHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);

  // tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
  unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
  glDrawBuffers(3, attachments);

  // create and attach depth buffer (renderbuffer)
  unsigned int rboDepth;
  glGenRenderbuffers(1, &rboDepth);
  glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, this->_windowWidth, this->_windowHeight);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

  // finally check if framebuffer is complete
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
  {
    printf("gbuffer program failed to compile");
    return false;
  }

  // restore default FBO
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  // shader configuration
  // --------------------
  shaderFSQBuffer->use();
  shaderFSQBuffer->setInt("gPosition", 0);
  shaderFSQBuffer->setInt("gNormal", 1);
  shaderFSQBuffer->setInt("gAlbedoSpec", 2);

  return true;
}

// Implement the Deferred Shading pipeline for rendering the OBJ files using FBO
void SceneSandbox::Render_Deferred_Objects(const GLint fbo)
{
  /////////////////////////////////////////////////////////////////////////////
  // First pass: Generate the G-buffer using multiple render targets.
  glBindFramebuffer(GL_FRAMEBUFFER, gBufferFBO);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glm::mat4 projection = glm::perspective(glm::radians(m_pCamera->Zoom), (float)this->_windowWidth / (float)this->_windowHeight, 0.1f, 100.0f);
  glm::mat4 view = m_pCamera->GetViewMatrix();
  glm::mat4 model = glm::mat4(1.0f);

  shaderGBuffer->use();
  shaderGBuffer->setMat4("projection", projection);
  shaderGBuffer->setMat4("view", view);

  for (auto pObj : pObjectList)
  {
    model = glm::mat4(1.0f);
    model = glm::translate(model, pObj->position);
    model = glm::scale(model, pObj->scale);
    shaderGBuffer->setMat4("model", model);
    shaderGBuffer->setVec3("color", pObj->color);
    shaderGBuffer->setInt("renderTarget", renderTarget);
    pObj->Draw(shaderGBuffer);
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  /////////////////////////////////////////////////////////////////////////////
  // Second Pass: Use the render targets from Pass 1 as input textures to the
  //              Lighting Pass. The Vertex Shader will be a straight pass -
  //              through to render a Full Screen Quad(FSQ).
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  shaderFSQBuffer->use();
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, gPosition);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, gNormal);
  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);

  // Uniform Blocks (as before) to pass light information.
  int lightIndex = 0;
  const float linear = 0.7;
  const float quadratic = 1.8;
  for (auto pLight : pLightList)
  {
    if (pLight->isActivated == false)
      continue;

    shaderFSQBuffer->setVec3("lights[" + std::to_string(lightIndex) + "].Position", pLight->position);
    shaderFSQBuffer->setVec3("lights[" + std::to_string(lightIndex) + "].Color", pLight->color);

    // update attenuation parameters
    shaderFSQBuffer->setFloat("lights[" + std::to_string(lightIndex) + "].Linear", linear);
    shaderFSQBuffer->setFloat("lights[" + std::to_string(lightIndex) + "].Quadratic", quadratic);

    ++lightIndex;
  }

  // send general light data
  shaderFSQBuffer->setInt("currLightsNum", numOfSpheres);
  shaderFSQBuffer->setVec3("viewPos", m_pCamera->Position);
  shaderFSQBuffer->setInt("renderTarget", renderTarget);

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // FSQ should be always randered!
  RenderFSQ(); // FSQBuffer shader on a quad (FSQ)
}

void SceneSandbox::Copy_Depth_Info(const GLint fbo, const GLint fboDefault)
{
  glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);        // from G-buffer
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fboDefault); // to the default buffer (0)

  // blit to default framebuffer.
  glBlitFramebuffer(0, 0, this->_windowWidth, this->_windowHeight, 0, 0, this->_windowWidth, this->_windowHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
  glBindFramebuffer(GL_FRAMEBUFFER, fboDefault);
}

// It renders all dubug data
void SceneSandbox::Render_Debug_Objects()
{
  glm::mat4 projection = glm::perspective(glm::radians(m_pCamera->Zoom), (float)this->_windowWidth / (float)this->_windowHeight, 0.1f, 100.0f);
  glm::mat4 view = m_pCamera->GetViewMatrix();
  glm::mat4 model = glm::mat4(1.0f);

  // Vertex normal
  if (showVertexNormal || showFaceNormal)
  {
    shaderNormals->use();
    shaderNormals->setMat4("projection", projection);
    shaderNormals->setMat4("view", view);

    for (auto pObj : pObjectList)
    {
      model = glm::mat4(1.0f);
      model = glm::translate(model, pObj->position);
      model = glm::scale(model, pObj->scale);
      shaderNormals->setMat4("model", model);
      shaderNormals->setVec3("color", DEBUG_GEOMETRY_COLOR);
      shaderNormals->setBool("showVertexNormal", showVertexNormal);
      shaderNormals->setBool("showFaceNormal", showFaceNormal);
      pObj->Draw(shaderNormals);
    }
  }

  // Lights (Spheres)
  shaderAmbient->use();
  shaderAmbient->setMat4("projection", projection);
  shaderAmbient->setMat4("view", view);

  int lightIndex = 0;
  float theta, x, z;
  for (auto pLight : pLightList)
  {
    if (pLight->isActivated == false)
      continue;

    // Makes spheres orbit
    {
      //get the current angle
      theta = 2.0f * glm::pi<float>() * float(lightIndex) / float(numOfSpheres);
      theta += angleOfLightObjOrbiting;

      x = spheresOrbitRadius * cosf(theta); //calculate the x component
      z = spheresOrbitRadius * sinf(theta); //calculate the y component

      pLight->position = glm::vec3(x, 0.f, z);

      ++lightIndex;
    }

    model = glm::mat4(1.0f);
    model = glm::translate(model, pLight->position);
    model = glm::scale(model, pLight->scale);
    shaderAmbient->setMat4("model", model);
    shaderAmbient->setVec3("color", pLight->color);
    pLight->Draw(shaderAmbient);
  }

  RenderBoundingVolumes();
}

void SceneSandbox::LoadShaders()
{
  shaderGBuffer = new Shader(
    "../Common/shaders/Deffered_G_Buffer.vs",
    "../Common/shaders/Deffered_G_Buffer.fs"
  );

  shaderFSQBuffer = new Shader(
    "../Common/shaders/Deffered_FSQ_Buffer.vs",
    "../Common/shaders/Deffered_FSQ_Buffer.fs"
  );

  shaderAmbient = new Shader(
    "../Common/shaders/Ambient.vs",
    "../Common/shaders/Ambient.fs"
  );

  shaderNormals = new Shader(
    "../Common/shaders/Ambient.vs",
    "../Common/shaders/Ambient.fs",
    "../Common/shaders/Debug_Normals.gs", InTriangles, OutLineStrip, 8
  );

  shaderLines = new Shader(
    "../Common/shaders/Line.vs",
    "../Common/shaders/Line.fs"
  );
}

void SceneSandbox::ClearShaders()
{
  delete shaderGBuffer;
  delete shaderFSQBuffer;
  delete shaderAmbient;
  delete shaderNormals;
  delete shaderLines;
}

void SceneSandbox::CreateCenterObject(std::vector<string> const& _obj_file_paths)
{
  m_prevObjPaths = _obj_file_paths;
  const bool isUnit = (_obj_file_paths.size() == 1) ? true : false;

  for (auto obj_file_path : _obj_file_paths)
  {
    Object* pObj = nullptr;
    if (isUnit)
      pObj = new Object(new Model(obj_file_path), false);
    else
      pObj = new Object(new Model(obj_file_path), 0.00001f, false);

    pObj->color = glm::vec3(1.f, 1.f, 1.f); // white color
    pObj->AddBoundingVolumes();
    pObjectList.push_back(pObj);
  }

  if (structureType == SP_BSP_TREE)
    pBSPTree = new BSPTree(pObjectList);
  else
    pAdaptiveOctree = new AdaptiveOctree(pObjectList);

  // Setup in this late line because of Spatial Partitioning
  for (auto& pObj : pObjectList)
    pObj->SetupMesh();

  // Bounding Volume Hierarchy
  pHierBottomUpAABB = new HierarchyBottomUpAABB(pObjectList);
  pHierBottomUpSphere = new HierarchyBottomUpSphere(pObjectList);
  pHierarchyTopDownAABB = new HierarchyTopDownAABB(pObjectList);
  pHierarchyTopDownSphere = new HierarchyTopDownSphere(pObjectList);

  // for Imgui config
  if (BVHT_BottomUp_AABB <= structureType && structureType <= BVHT_TopDown_Sphere)
    maxTreeHeight = MAX_BVTREE_HEIGHT;
  else if (structureType == SP_AdaptiveOctree)
    maxTreeHeight = pAdaptiveOctree->GetMaxDepth();
}

void SceneSandbox::DeleteCenterObject()
{
  if (pAdaptiveOctree != nullptr)
  {
    delete pAdaptiveOctree;
    pAdaptiveOctree = nullptr;
  }

  if (pBSPTree != nullptr)
  {
    delete pBSPTree;
    pBSPTree = nullptr;
  }

  // Bounding Volume Hierarchy
  delete pHierBottomUpAABB;
  pHierBottomUpAABB = nullptr;

  delete pHierBottomUpSphere;
  pHierBottomUpSphere = nullptr;

  delete pHierarchyTopDownAABB;
  pHierarchyTopDownAABB = nullptr;

  delete pHierarchyTopDownSphere;
  pHierarchyTopDownSphere = nullptr;

  // std::list<Object*> pObjectList
  for (auto pObj : pObjectList)
  {
    delete pObj;
    pObj = nullptr;
  }

  pObjectList.clear();
  m_prevObjPaths.clear();
}

// It renders a quad in NDC
unsigned int FSQ_VAO = 0;
unsigned int FSQ_VBO;
void SceneSandbox::RenderFSQ()
{
  if (FSQ_VAO == 0)
  {
    float quadVertices[] = {
      // positions        // texture Coords
      -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
      -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
       1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
       1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
    };

    // setup a quad VAO
    glGenVertexArrays(1, &FSQ_VAO);
    glGenBuffers(1, &FSQ_VBO);

    glBindVertexArray(FSQ_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, FSQ_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0); // positions
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1); // texture Coords
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
  }

  glBindVertexArray(FSQ_VAO);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  glBindVertexArray(0);
}

void SceneSandbox::RenderBoundingVolumes()
{
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  glm::mat4 projection = glm::perspective(glm::radians(m_pCamera->Zoom), (float)this->_windowWidth / (float)this->_windowHeight, 0.1f, 100.0f);
  glm::mat4 view = m_pCamera->GetViewMatrix();
  glm::mat4 model = glm::mat4(1.0f);

  // Bounding Volumes
  shaderLines->use();
  shaderLines->setMat4("projection", projection);
  shaderLines->setMat4("view", view);
  for (auto pObject : pObjectList)
  {
    for (auto pBVolume : pObject->GetBVolumeList())
    {
      if (pBVolume->bvType != boundingVolumeType)
        continue;

      model = glm::mat4(1.0f);
      model = glm::translate(model, glm::vec3(0.f));
      model = glm::scale(model, glm::vec3(1.f));
      shaderLines->setMat4("model", model);
      shaderLines->setVec3("color", glm::vec3(1.f, 0.f, 0.f));

      pBVolume->Draw(shaderLines);
    }
  }

  // Hierarchy
  switch (structureType)
  {
  case BVHT_BottomUp_AABB:
    pHierBottomUpAABB->Draw(shaderLines);
    break;
  case BVHT_BottomUp_Sphere:
    pHierBottomUpSphere->Draw(shaderLines);
    break;
  case BVHT_TopDown_AABB:
    pHierarchyTopDownAABB->Draw(shaderLines);
    break;
  case BVHT_TopDown_Sphere:
    pHierarchyTopDownSphere->Draw(shaderLines);
    break;
    // Spatial Partitioning is representing by directly chainging vertices color.
    // Ref. MaxDepth in Deffered_G_Buffer.fs
  case SP_AdaptiveOctree:
    pAdaptiveOctree->Draw(shaderLines); // It's for showing volumes
    break;
  default:
    break;
  }

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}


void SceneSandbox::RenderImGui()
{
  /////////////////////////////////////////////////////////////////////////////
  // General Control Panel ////////////////////////////////////////////////////

  ImGui::Begin("General Control Panel");

  // Open OBJ File
  {
    ImGui::PushItemFlag(ImGuiItemFlags_Disabled, (bool)s_pPFDOpenFile);
    if (ImGui::Button("Load a Model"))
    {
      char buf[256];
      GetCurrentDirectoryA(256, buf);
      std::string path(buf);
      path.erase(path.end() - sizeof("Advanced_Algorithm"), path.end());
      path += std::string("\\Common\\models");

      s_pPFDOpenFile = std::make_shared<pfd::open_file>("Select a model file to load", path, std::vector<std::string>{ "All Files", "*.*" });
    }

    if (s_pPFDOpenFile && s_pPFDOpenFile->ready())
    {
      auto result = s_pPFDOpenFile->result();
      if (result.size())
      {
        m_openedOBJFilePath = *(result.begin());

        std::cout << "Opened a file: " << m_openedOBJFilePath << "\n";

        DeleteCenterObject();
        CreateCenterObject(std::vector<std::string>(1, m_openedOBJFilePath.c_str()));
      }

      s_pPFDOpenFile = nullptr;
    }

    ImGui::SameLine();
    ImGui::Text("Path: %s", m_openedOBJFilePath.c_str());
  }

  // Open Folder
  // It recursively scans the target directory to load all model files
  {
    ImGui::PushItemFlag(ImGuiItemFlags_Disabled, (bool)s_pPFDOpenFolder);
    if (ImGui::Button("Load PowerPlant Folder"))
    {
      char buf[256];
      GetCurrentDirectoryA(256, buf);
      std::string path(buf);
      path.erase(path.end() - sizeof("Advanced_Algorithm"), path.end());
      path += std::string("\\Common\\models");

      s_pPFDOpenFolder = std::make_shared<pfd::select_folder>("Select folder to load", path);
    }

    if (s_pPFDOpenFolder && s_pPFDOpenFolder->ready())
    {
      string result = s_pPFDOpenFolder->result();
      if (result.size())
      {
        m_openedFolderPath = result;

        std::cout << "Opened folder: " << m_openedFolderPath << "\n";

        std::vector<std::string> filePaths;
        for (const auto& entry : std::filesystem::recursive_directory_iterator(m_openedFolderPath))
        {
          // only load a file, not a directory
          if (entry.is_directory() == false)
            filePaths.push_back(entry.path().string());
        }

        DeleteCenterObject();
        CreateCenterObject(filePaths);
      }

      s_pPFDOpenFolder = nullptr;
    }

    ImGui::SameLine();
    ImGui::Text("Path: %s", m_openedFolderPath.c_str());

    ImGui::Dummy(ImVec2(0.0f, 20.0f));
  }

  // Configuration for debug
  {
    ImGui::RadioButton("Geometry", &renderTarget, 0); ImGui::SameLine();
    ImGui::RadioButton("Position", &renderTarget, 1); ImGui::SameLine();
    ImGui::RadioButton("Normals", &renderTarget, 2); ImGui::SameLine();
    ImGui::RadioButton("Albedo", &renderTarget, 3); ImGui::SameLine();
    ImGui::RadioButton("Specular", &renderTarget, 4); ImGui::SameLine();
    ImGui::RadioButton("Lighting", &renderTarget, 5);  ImGui::SameLine();
    ImGui::RadioButton("Each Node Color", &renderTarget, 6);

    // Geometry = Polygon Mode
    if (renderTarget == 0)
      isPolygonMode = true;
    else
      isPolygonMode = false;

    if (ImGui::Checkbox("Enable copy depth info", &bCopyDepthInfo))
    {
      if (bCopyDepthInfo == false)
      {
        showVertexNormal = false;
        showFaceNormal = false;
      }
    }

    ImGui::SameLine();
    if (ImGui::Checkbox("Show Vertex Normal", &showVertexNormal))
    {
      if (bCopyDepthInfo == false)
        bCopyDepthInfo = true;
    }

    ImGui::SameLine();
    if (ImGui::Checkbox("Show Face Normal", &showFaceNormal))
    {
      if (bCopyDepthInfo == false)
        bCopyDepthInfo = true;
    }

    ImGui::Dummy(ImVec2(0.0f, 20.0f));

    if (ImGui::Combo("Bounding Volume Type", &boundingVolumeType,
      "None\0AABB, Box\0Centroid Method, Sphere\0Ritter's Method, Sphere\0Larsson's Method, Sphere\0PCA, Sphere\0PCA, Ellipsoid\0OBB - PCA, Box"))
    {
      if (boundingVolumeType != BVT_None)
        structureType = ST_None;
    }

    if (ImGui::Combo("Partitioning Structure Type", &structureType,
      "None\0BottomUp AABB\0BottomUp Sphere\0TopDown AABB\0TopDown Sphere\0Adaptive Octree\0BSP Tree"))
    {
      if (structureType != ST_None)
        boundingVolumeType = BVT_None;

      if (BVHT_BottomUp_AABB <= structureType && structureType <= BVHT_TopDown_Sphere)
        maxTreeHeight = MAX_BVTREE_HEIGHT;
      else if (structureType == SP_AdaptiveOctree)
      {
        if (pAdaptiveOctree == nullptr)
        {
          std::vector<string> prevObjPaths = m_prevObjPaths;
          DeleteCenterObject();
          CreateCenterObject(prevObjPaths);
        }

        maxTreeHeight = pAdaptiveOctree->GetMaxDepth();
        renderTarget = 6;
      }
      else if (structureType == SP_BSP_TREE)
      {
        if (pBSPTree == nullptr)
        {
          std::vector<string> prevObjPaths = m_prevObjPaths;
          DeleteCenterObject();
          CreateCenterObject(prevObjPaths);
        }
      }
    }

    if (ImGui::SliderInt("Hierarchy Level", &structureTreeLevel, -1, maxTreeHeight))
    {
      pHierBottomUpAABB->targetTreeHeight = structureTreeLevel;
      pHierBottomUpSphere->targetTreeHeight = structureTreeLevel;
      pHierarchyTopDownAABB->targetTreeHeight = structureTreeLevel;
      pHierarchyTopDownSphere->targetTreeHeight = structureTreeLevel;

      if (pAdaptiveOctree != nullptr)
        pAdaptiveOctree->targetTreeHeight = structureTreeLevel;
    }

    ImGui::Dummy(ImVec2(0.0f, 20.0f));
  }

  ImGui::Checkbox("Orbiting Lights", &isLightObjOrbiting); ImGui::SameLine();
  ImGui::SliderFloat("orb. radian", &angleOfLightObjOrbiting, 0.0f, glm::two_pi<float>());
  if (ImGui::SliderInt("Number of Lights", &numOfSpheres, 0, MAX_LIGHT_NUMBER))
  {
    std::list<Object*>::iterator lightIter = pLightList.begin();
    for (int i = 0; lightIter != pLightList.end(); ++lightIter, ++i)
    {
      if (i < numOfSpheres)
        (*lightIter)->isActivated = true;
      else
        (*lightIter)->isActivated = false;
    }
  }

  ImGui::End();
}