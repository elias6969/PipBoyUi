#include "ui/glyphs.h"
#include "core/shader.h"
#include "util/variables.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <stdexcept>
#include <iostream>

TextRenderer::TextRenderer(const char* fontPath, int pixelHeight)
{
    FT_Library ft;
    FT_Face face;

    if (FT_Init_FreeType(&ft))
        throw std::runtime_error("Failed to initialize FreeType");

    if (FT_New_Face(ft, fontPath, 0, &face))
        throw std::runtime_error("Failed to load font");

    FT_Set_Pixel_Sizes(face, 0, pixelHeight);

    // --- Build atlas (simple row packing) ---
    const int atlasWidth = 1024;
    const int atlasHeight = 1024;

    unsigned char* atlas = new unsigned char[atlasWidth * atlasHeight];
    memset(atlas, 0, atlasWidth * atlasHeight);

    int penX = 0;
    int penY = 0;
    int rowHeight = 0;

    for (unsigned char c = 0; c < 128; c++)
    {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
            continue;

        if (penX + face->glyph->bitmap.width >= atlasWidth)
        {
            penX = 0;
            penY += rowHeight;
            rowHeight = 0;
        }

        for (int y = 0; y < face->glyph->bitmap.rows; y++)
        {
            memcpy(
                atlas + (penY + y) * atlasWidth + penX,
                face->glyph->bitmap.buffer + y * face->glyph->bitmap.pitch,
                face->glyph->bitmap.width
            );
        }

        glyphs[c] = {
            float(penX) / atlasWidth,
            float(penY) / atlasHeight,
            float(penX + face->glyph->bitmap.width) / atlasWidth,
            float(penY + face->glyph->bitmap.rows) / atlasHeight,
            static_cast<int>(face->glyph->bitmap.width),
            static_cast<int>(face->glyph->bitmap.rows),
            face->glyph->bitmap_left,
            face->glyph->bitmap_top,
            static_cast<int>(face->glyph->advance.x)
        };

        penX += face->glyph->bitmap.width;
        rowHeight = std::max(rowHeight, static_cast<int>(face->glyph->bitmap.rows));
    }

    // Upload atlas
    glGenTextures(1, &atlasTexture);
    glBindTexture(GL_TEXTURE_2D, atlasTexture);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, atlasWidth, atlasHeight, 0, GL_RED, GL_UNSIGNED_BYTE, atlas);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    delete[] atlas;

    // Create shared VAO/VBO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4 * 256, nullptr, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    FT_Done_Face(face);
    FT_Done_FreeType(ft);
}


glm::vec2 TextRenderer::measure(const std::string& text) const
{
    float width = 0.0f;
    float maxWidth = 0.0f;

    int maxAscent = 0;
    int maxDescent = 0;

    for (char c : text)
    {
        if (c == '\n') {
            maxWidth = std::max(maxWidth, width);
            width = 0.0f;
            continue;
        }

        const Glyph& g = glyphs[(unsigned char)c];

        maxAscent  = std::max(maxAscent,  g.bearingY);
        maxDescent = std::max(maxDescent, g.height - g.bearingY);

        width += (g.advance >> 6);
    }

    maxWidth = std::max(maxWidth, width);

    float height = float(maxAscent + maxDescent);

    return { maxWidth, height };
}

void TextRenderer::setProjection(float width, float height)
{
    projection = glm::ortho(0.0f, width, 0.0f, height);
}

void TextRenderer::render(const std::string& text, float x, float y)
{
    if (!shader) return;

    shader->use();
    shader->setUniform("textColor", glm::vec3(0.0f, 1.0f, 0.0f));
    shader->setUniform("projection", projection);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, atlasTexture);

    glBindVertexArray(VAO);

    std::vector<float> vertexBuffer;
    vertexBuffer.reserve(text.size() * 6 * 4);

    float xpos = x;

    for (char c : text)
    {
        const Glyph& g = glyphs[(unsigned char)c];

        float w = (float)g.width;
        float h = (float)g.height;

        float xpos2 = xpos + g.bearingX;
        float ypos2 = y - (g.height - g.bearingY);

        float verts[] = {
            xpos2,     ypos2 + h, g.u0, g.v0,
            xpos2,     ypos2,     g.u0, g.v1,
            xpos2 + w, ypos2,     g.u1, g.v1,

            xpos2,     ypos2 + h, g.u0, g.v0,
            xpos2 + w, ypos2,     g.u1, g.v1,
            xpos2 + w, ypos2 + h, g.u1, g.v0
        };

        vertexBuffer.insert(vertexBuffer.end(), verts, verts + 24);

        xpos += (g.advance >> 6);
    }

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertexBuffer.size() * sizeof(float), vertexBuffer.data());

    glDrawArrays(GL_TRIANGLES, 0, vertexBuffer.size() / 4);
}

TextRenderer::~TextRenderer()
{
    glDeleteTextures(1, &atlasTexture);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
}
