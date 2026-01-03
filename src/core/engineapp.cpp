#include "core/engine_app.h"
#include "util/variables.h"
#include <glm/glm.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <memory>

bool EngineApp::init() {
  std::cout << "Engine init\n";

  textShader = std::make_unique<Shader>();
  textShader->LoadShaders((PathManager::shaderPath + "text.vert").c_str(),
                          (PathManager::shaderPath + "text.frag").c_str());

  textRenderer = std::make_unique<TextRenderer>(
      (PathManager::fontpath + "monofonto/monofontorg.otf").c_str(), 32);
  textRenderer->setShader(textShader.get());

  textsomething = std::make_unique<TextRenderer>(
      (PathManager::fontpath + "monofonto/monofontorg.otf").c_str(), 32);
  textsomething->setShader(textShader.get());

  testButton = std::make_unique<UIButton>();
  testButton->setPosition(200.0f, 200.0f);
  testButton->setPadding(40.0f, 400.0f);
  testButton->setText("Click Me");

  return true;
}

bool EngineApp::running() { return true; }

void EngineApp::update(GLFWwindow *window) {
  textRenderer->setProjection(PathManager::SCR_WIDTH, PathManager::SCR_HEIGHT);
  textsomething->setProjection(PathManager::SCR_WIDTH, PathManager::SCR_HEIGHT);

  textRenderer->render("Hello", 25.0f, PathManager::SCR_HEIGHT - 50.0f);
  textsomething->render("nope", 25.0f, PathManager::SCR_HEIGHT - 150.0f);

  mouseDown =
      glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
  glfwGetCursorPos(window, &msX, &msY);
  testButton->update(static_cast<float>(msX), static_cast<float>(msY), mouseDown);
  testButton->render();
}

void EngineApp::shutdown() {
  textRenderer.reset();
  textsomething.reset();
  textShader.reset();
}
