#version 410 core

uniform vec2 u_resolution;
in vec3 v_vertexColors;
out vec4 color;

void main() {
    color = vec4(v_vertexColors.r, v_vertexColors.g, v_vertexColors.b, 1.0);
}
