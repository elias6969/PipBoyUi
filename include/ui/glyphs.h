#ifndef GLYPHS_H
#define GLYPHS_H

#include <glm/glm.hpp>
#include <string>
#include <vector>

class Shader;

struct Glyph {
  float u0, v0;
  float u1, v1;

  int width, height;
  int bearingX, bearingY;
  int advance;
};

class TextRenderer {
public:
  TextRenderer(const char *fontPath, int pixelHeight);
  ~TextRenderer();

  void setShader(Shader *s) { shader = s; }

  // One draw call per string
  void render(const std::string &text, float x, float y);

  // Must be called on window resize
  void setProjection(float width, float height);

  glm::vec2 measure(const std::string& text) const;

private:
  Glyph glyphs[128];

  unsigned int VAO, VBO;
  unsigned int atlasTexture;

  Shader *shader = nullptr;
  glm::mat4 projection;
};

#endif
