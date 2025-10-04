#version 430

uniform vec4 color;
uniform sampler2D face;

in data {
  vec2 textcoord;
}v;

out vec4 outcolor;

void main (void)
{
  outcolor = color*texture(face, v.textcoord);
}
