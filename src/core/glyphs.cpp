#include "ft2build.h"
#include FT_FREETYPE_H
#include "core/Shader.h"
#include "ui/glyphs.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <stdexcept>
#include <string>

Text::Text(const char *fontPath, int pixelHeight) {
  if (FT_Init_FreeType(&library))
    throw std::runtime_error("Failed to initialize FreeType");

  if (FT_New_Face(library, fontPath, 0, &face))
    throw std::runtime_error("Failed to load font");

  FT_Set_Pixel_Sizes(face, 0, pixelHeight);

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1); 

  for (unsigned char c = 0; c < 128; c++) {
    if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
      std::cout << "Failed to load glyph: " << c << std::endl;
      continue;
    }

    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width,
                 face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE,
                 face->glyph->bitmap.buffer);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    Character character = {texture,
                           static_cast<int>(face->glyph->bitmap.width),
                           static_cast<int>(face->glyph->bitmap.rows),
                           face->glyph->bitmap_left,
                           face->glyph->bitmap_top,
                           static_cast<unsigned int>(face->glyph->advance.x)};

    Characters.insert(std::pair<char, Character>(c, character));
  }

  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

void Text::render(const std::string &text, float x, float y) {
  if (!shader) {
    std::cerr << "Text::render() ERROR — no shader set!" << std::endl;
    return;
  }

  shader->use();
  shader->setUniform("textColor", glm::vec3(0.0f, 1.0f, 0.0f)); // Pip-Boy green
  glActiveTexture(GL_TEXTURE0);

  glBindVertexArray(VAO);

  for (char c : text) {
    Character ch = Characters[c];

    float xpos = x + ch.bearingX;
    float ypos = y - (ch.height - ch.bearingY);

    float w = (float)ch.width;
    float h = (float)ch.height;

    float vertices[6][4] = {
        {xpos, ypos + h, 0.0f, 0.0f},    {xpos, ypos, 0.0f, 1.0f},
        {xpos + w, ypos, 1.0f, 1.0f},

        {xpos, ypos + h, 0.0f, 0.0f},    {xpos + w, ypos, 1.0f, 1.0f},
        {xpos + w, ypos + h, 1.0f, 0.0f}};

    glBindTexture(GL_TEXTURE_2D, ch.TextureID);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    x += (ch.advance >> 6);
  }

  glBindVertexArray(0);
  glBindTexture(GL_TEXTURE_2D, 0);
}

Text::~Text() {
  FT_Done_Face(face);
  FT_Done_FreeType(library);
}
