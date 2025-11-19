#version 330 core

in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D text;     // glyph texture (GL_RED)
uniform vec3 textColor;     // pip-boy green

void main()
{
    float alpha = texture(text, TexCoords).r;  // FreeType uses red channel
    FragColor = vec4(textColor, alpha);
}
