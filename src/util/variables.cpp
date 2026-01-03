#include "util/variables.h"
#include "util/filemanager.h"
#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>

VirtualFileSystem vfs("../assets");

// Define the variables.
std::string PathManager::texturePath = vfs.getFullPath("textures/");
std::string PathManager::shaderPath = vfs.getFullPath("shaders/");
std::string PathManager::cubemappath = vfs.getFullPath("cubemap/");
std::string PathManager::fontpath = vfs.getFullPath("fonts/");
std::string PathManager::modelPath = vfs.getFullPath("models/");
int PathManager::SCR_HEIGHT = 600;
int PathManager::SCR_WIDTH = 800;

void ScreenSizeConfiguration(GLFWwindow *window, int &width, int &height) {
  glfwGetFramebufferSize(window, &width, &height);
  PathManager::SCR_WIDTH = width;
  PathManager::SCR_HEIGHT = height;
}
