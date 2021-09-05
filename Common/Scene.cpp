#include "Scene.h"

#include <memory> // shared_ptr

// "public static" control variables by imgui
std::shared_ptr<pfd::open_file> Scene::s_pPFDOpenFile = nullptr;
std::shared_ptr<pfd::select_folder> Scene::s_pPFDOpenFolder = nullptr;
int Scene::isGPUCalculation = true;
int Scene::isNormalCalculation = false;
bool Scene::showVertexNormal = false;
bool Scene::showFaceNormal = false;

Scene::Scene() : _windowWidth(100), _windowHeight(100)
{

}

Scene::Scene(int windowWidth, int windowHeight)
{
    _windowHeight = windowHeight;
    _windowWidth = windowWidth;
}

Scene::~Scene()
{
    CleanUp();
}

// Public methods

// Init: called once when the scene is initialized
int Scene::Init()
{
    return -1;
}

// LoadAllShaders: This is the placeholder for loading the shader files
void Scene::LoadAllShaders()
{
    return;
}


// preRender : called to setup stuff prior to rendering the frame
int Scene::preRender(const float dt)
{
    return -1;
}

// Render : per frame rendering of the scene
int Scene::Render(const float dt)
{
    return -1;
}

// postRender : Any updates to calculate after current frame
int Scene::postRender(const float dt)
{
    return -1;
}

// CleanUp : clean up resources before destruction
void Scene::CleanUp()
{
    return;
}

// Display : Per-frame execution of the scene
int Scene::Display(const float dt)
{
    preRender(dt);

    Render(dt);

    postRender(dt);

    return -1;
}