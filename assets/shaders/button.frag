#version 330 core

out vec4 FragColor;

in vec2 vLocalPos;

uniform vec2 size;
uniform vec3 color;      // base green from CPU
uniform float radius;
uniform float edgeSoft;
uniform float time;      // optional, for subtle animation

// Signed distance for rounded rect
float roundedRect(vec2 p, vec2 halfSize, float r)
{
    vec2 q = abs(p) - halfSize + vec2(r);
    return length(max(q, 0.0)) - r;
}

// Cheap scanline effect
float scanlines(float y)
{
    return 0.85 + 0.15 * sin(y * 0.25);
}

void main()
{
    vec2 centered = vLocalPos - size * 0.5;

    float dist = roundedRect(centered, size * 0.5, radius);
    float alpha = 1.0 - smoothstep(0.0, edgeSoft, dist);
    if (alpha <= 0.0)
        discard;

    // Base phosphor color
    vec3 base = color;

    // Inner glow (brighter near center)
    float glow = 1.0 - smoothstep(0.0, size.x * 0.6, length(centered));
    base += glow * vec3(0.05, 0.25, 0.05);

    // Scanlines (vertical CRT feel)
    float scan = scanlines(gl_FragCoord.y);
    base *= scan;

    // Slight edge darkening
    float edge = smoothstep(-2.0, 6.0, dist);
    base *= mix(1.2, 0.75, edge);

    // Final color
    FragColor = vec4(base, alpha);
}

