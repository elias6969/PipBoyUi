#ifndef engine_app_h
#define engine_app_h

#include "core/shader.h"
#include "ui/glyphs.h"
#include "ui/UIButton.h"
#include <memory>
#include <GLFW/glfw3.h>

class EngineApp {
public:
  bool init();
  bool running();
  void update(GLFWwindow *window);
  void shutdown();
  void onwindowresize(int width, int height);

private:
  double msX, msY;
  bool mouseDown = false;
  std::unique_ptr<Shader> textShader;
  std::unique_ptr<TextRenderer> textRenderer;
  std::unique_ptr<TextRenderer> textsomething;
  std::unique_ptr<UIButton> testButton;
};

#endif
