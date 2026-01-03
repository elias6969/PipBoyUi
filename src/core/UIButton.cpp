#include "ui/UIButton.h"

#include "ui/glyphs.h"
#include "util/variables.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/ext/matrix_clip_space.hpp>

UIButton::UIButton() {
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 12, nullptr, GL_DYNAMIC_DRAW);

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  textRenderer = std::make_unique<TextRenderer>(
      (PathManager::fontpath + "monofonto/monofontorg.otf").c_str(), 32);

  buttonShader.LoadShaders((PathManager::shaderPath + "button.vert").c_str(),
                           (PathManager::shaderPath + "button.frag").c_str());
  textShader.LoadShaders((PathManager::shaderPath + "text.vert").c_str(),
                         (PathManager::shaderPath + "text.frag").c_str());
  textRenderer->setShader(&textShader);
}

UIButton::~UIButton() {
  glDeleteBuffers(1, &VBO);
  glDeleteVertexArrays(1, &VAO);
}

void UIButton::setText(const std::string &t) {
  text = t;
  recalcLayout();
}

void UIButton::setPosition(float x, float y) {
  pos = {x, y};
  updateQuad();
}

void UIButton::setPadding(float px, float py) {
  padding = {px, py};
  recalcLayout();
}

void UIButton::recalcLayout() {
  textSize = textRenderer->measure(text);
  size = textSize + padding * 2.0f;
  updateQuad();
}

void UIButton::updateQuad() {
  float x = pos.x;
  float y = pos.y;
  float w = size.x;
  float h = size.y;

  float verts[] = {x, y, x + w, y,     x + w, y + h,

                   x, y, x + w, y + h, x,     y + h};

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verts), verts);
}

void UIButton::update(float mx, float my, bool mouseDown) {
  hovered = mx >= pos.x && mx <= pos.x + size.x && my >= pos.y &&
            my <= pos.y + size.y;

  pressed = hovered && mouseDown;
}

void UIButton::render() {
  if (!buttonShader.ID)
    return;

  buttonShader.use();

  float intensity = pressed ? 0.35f : hovered ? 0.55f : 0.25f;
  glm::mat4 ortho =
      glm::ortho(0.0f, (float)PathManager::SCR_WIDTH,
                 (float)PathManager::SCR_HEIGHT, 0.0f, -1.0f, 1.0f);

  buttonShader.setUniform("time", static_cast<float>(glfwGetTime()));
  buttonShader.setUniform("uProjection", ortho);
  buttonShader.setUniform("color", glm::vec3(0.1f, intensity, 0.2f));
  buttonShader.setUniform("uPos", pos);
  buttonShader.setUniform("size", size);
  buttonShader.setUniform("radius", 6.0f);
  buttonShader.setUniform("edgeSoft", 1.0f);

  glBindVertexArray(VAO);
  glDrawArrays(GL_TRIANGLES, 0, 6);

  float textX = pos.x + padding.x;
  float textY =
    PathManager::SCR_HEIGHT
    - (pos.y + padding.y + textSize.y);

  textRenderer->setProjection(PathManager::SCR_WIDTH, PathManager::SCR_HEIGHT);
  textRenderer->render(text, textX, textY);
}
