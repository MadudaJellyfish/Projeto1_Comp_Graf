#version 430

layout (location=0) in vec4 vertex;
layout (location=1) in vec2 textcoord;

uniform mat4 Mvp;

out data {
  vec2 textcoord;
} v;

void main (void)
{
  v.textcoord = textcoord;
  gl_Position = Mvp * vertex;
  //gl_Position = vertex;
}

