#version 330 core

layout (location = 0) in vec2 aPos;

uniform mat4 uProjection;
uniform vec2 uPos;
uniform vec2 uSize;

out vec2 vLocalPos;

void main()
{
    vLocalPos = aPos - uPos; // local space (0 → size)
    gl_Position = uProjection * vec4(aPos, 0.0, 1.0);
}

