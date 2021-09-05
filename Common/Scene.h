#ifndef SCENE_H
#define SCENE_H

// openGL
#include <GL/glew.h>
#include <GLFW/glfw3.h>

//pfd
#include <pfd/portable-file-dialogs.h>

#define _GET_GL_ERROR   { GLenum err = glGetError(); std::cout << "[OpenGL Error] " << glewGetErrorString(err) << std::endl; }

class Scene
{

public:
  ////////////////////////////////////
  // static control variables by imgui

  static std::shared_ptr<pfd::open_file> s_pPFDOpenFile;
  static std::shared_ptr<pfd::select_folder> s_pPFDOpenFolder;
  static int isGPUCalculation;
  static int isNormalCalculation;
  static bool showVertexNormal;
  static bool showFaceNormal;

  Scene();
  Scene(int windowWidth, int windowHeight);
  virtual ~Scene();

  // Public methods

  // Init: called once when the scene is initialized
  virtual int Init();

  // LoadAllShaders: This is the placeholder for loading the shader files
  virtual void LoadAllShaders();

  // Display : encapsulates per-frame behavior of the scene
  virtual int Display(const float dt);

  // preRender : called to setup stuff prior to rendering the frame
  virtual int preRender(const float dt);

  // Render : per frame rendering of the scene
  virtual int Render(const float dt);

  // postRender : Any updates to calculate after current frame
  virtual int postRender(const float dt);

  // cleanup before destruction
  virtual void CleanUp();

protected:
  int _windowHeight, _windowWidth;
};


#endif
