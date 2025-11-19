#ifndef GLYPHS_H
#define GLYPHS_H

#include <map>
#include <string>

// FreeType opaque pointer forward declarations
typedef struct FT_LibraryRec_ *FT_Library;
typedef struct FT_FaceRec_ *FT_Face;

// Forward declare the Shader class — NOT a typedef
class Shader;

struct Character {
  unsigned int TextureID;
  int width, height;
  int bearingX, bearingY;
  unsigned int advance;
};

class Text {
public:
  Text(const char *fontPath, int pixelHeight);
  void render(const std::string &text, float x, float y);
  ~Text();

  void setShader(Shader *s) { shader = s; }

private:
  std::map<char, Character> Characters;
  unsigned int VAO, VBO;
  FT_Library library;
  FT_Face face;
  Shader *shader;
};

#endif
