// Include standard headers
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <iostream>
#include <cstring>
#include <fstream>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>

// Include GLM
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

// imgui
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

// Local / project headers
#include "../Common/Scene.h"
#include "../Common/Shader.hpp"
#include "Camera.h"
#include "SceneSandbox.h"

// Function declarations
void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

//////////////////////////////////////////////////////////////////////

GLFWwindow* window;
Scene* scene;
Camera* camera;

int windowWidth = 1440;
int windowHeight = 900;

float lastX = windowWidth / 2.0f;
float lastY = windowHeight / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
int main()
{
  // Initialise GLFW
  if (!glfwInit())
  {
    fprintf(stderr, "Failed to initialize GLFW\n");
    getchar();
    return -1;
  }

  // Setting up OpenGL properties
  const char* glsl_version = "#version 330";
  glfwWindowHint(GLFW_SAMPLES, 1); // change for anti-aliasing
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); // 4
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); // 6
  // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // Open a window and create its OpenGL context
  window = glfwCreateWindow(windowWidth, windowHeight, // window dimensions
    "OpenGL Graphics Pipeline", // window title
    nullptr, // which monitor (if full-screen mode)
    nullptr); // if sharing context with another window
  if (window == nullptr)
  {
    fprintf(stderr,
      "Failed to open GLFW window. If you have an Intel GPU, they are not 4.0 compatible.\n");
    getchar();
    glfwTerminate();
    return -1;
  }

  // OpenGL resource model - "glfwCreateWindow" creates the necessary data storage for the OpenGL
  // context but does NOT make the created context "current". We MUST make it current with the following
  // call
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
  glfwSetScrollCallback(window, scrollCallback);

  // Initialize GLEW
  glewExperimental = static_cast<GLboolean>(true); // Needed for core profile
  if (glewInit() != GLEW_OK)
  {
    fprintf(stderr, "Failed to initialize GLEW\n");
    getchar();
    glfwTerminate();
    return -1;
  }

  // Ensure we can capture the escape key being pressed below
  //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

///////////////////////////////////////////////////////////////////////////
// imgui

// Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO(); (void)io;
  //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
  //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

  // Setup Dear ImGui style
  ImGui::StyleColorsLight();

  // Setup Platform/Renderer bindings
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init(glsl_version);

  ///////////////////////////////////////////////////////////////////////////

    // Initialize the camrea and the scene
  camera = new Camera();
  scene = new SceneSandbox(windowWidth, windowHeight, camera);

  // Scene::Init encapsulates setting up the geometry and the texture
  // information for the scene
  scene->Init();

  do
  {
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    processInput(window);

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Now render the scene
    // Scene::Display method encapsulates pre-, render, and post- rendering operations
    scene->Display(deltaTime);

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window); // Swap buffers
    glfwPollEvents();        // Poll IO events

  } // Check if the ESC key was pressed or the window was closed
  while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
    glfwWindowShouldClose(window) == 0);

  ///////////////////////////////////////////////////////////////////////////
  // Deallocates objects

  delete scene;
  delete camera;

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwTerminate(); // Close OpenGL window and terminate GLFW

  return 0;
}

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
  (0, 0, width, height);
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
  if (firstMouse)
  {
    lastX = xpos;
    lastY = ypos;
    firstMouse = false;
  }

  float xoffset = xpos - lastX;
  float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

  lastX = xpos;
  lastY = ypos;

  camera->ProcessMouseMovement(xoffset, yoffset);
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
  camera->ProcessMouseScroll(yoffset);
}

void processInput(GLFWwindow* window)
{
  // exit
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);

  // movement
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    camera->ProcessKeyboard(FORWARD, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    camera->ProcessKeyboard(BACKWARD, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    camera->ProcessKeyboard(LEFT, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    camera->ProcessKeyboard(RIGHT, deltaTime);

  // view (aim)
  if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    camera->ProcessMouseMovement(+1.f, 0.f);
  if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    camera->ProcessMouseMovement(-1.f, 0.f);
  if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    camera->ProcessMouseMovement(0.f, -1.f);
  if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    camera->ProcessMouseMovement(0.f, +1.f);
}