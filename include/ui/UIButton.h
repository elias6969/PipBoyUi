#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <string>

#include "core/shader.h"

class TextRenderer;

class UIButton {
public:
  UIButton();
  ~UIButton();

  void setPosition(float x, float y);
  void setPadding(float px, float py);
  void setText(const std::string &t);

  void update(float mouseX, float mouseY, bool mouseDown);
  void render();

  bool isHovered() const { return hovered; }
  bool isPressed() const { return pressed; }

private:
  void recalcLayout();
  void updateQuad();

  std::unique_ptr<TextRenderer> textRenderer;
  std::string text;

  glm::vec2 pos{0.0f};
  glm::vec2 size{0.0f};
  glm::vec2 padding{10.0f, 6.0f};
  glm::vec2 textSize{0.0f};

  bool hovered{false};
  bool pressed{false};

  GLuint VAO{0};
  GLuint VBO{0};

  Shader buttonShader;
  Shader textShader;
};
