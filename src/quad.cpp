#include "quad.h"
#include "error.h"
#include "grid.h"

#include <iostream>

#ifdef _WIN32
<<<<<<< HEAD
//#define GLAD_GL_IMPLEMENTATION // Necessary for headeronly version.
#include <glad/glad.h>
=======
#include <glad/glad.h>

#elif __APPLE__
#include <OpenGL/gl3.h>

#elif __linux__
#include <glad/glad.h>
#endif

QuadPtr Quad::Make (int nx, int ny, float dis)
{
  return QuadPtr(new Quad(nx,ny,dis));
}

Quad::Quad (int nx, int ny, float dis)
{
  GridPtr grid = Grid::Make(nx,ny,dis);
  m_nind = grid->IndexCount();
  // create VAO
  glGenVertexArrays(1,&m_vao);
  glBindVertexArray(m_vao);
  // create coord buffer
  GLuint coord_buffer;
  glGenBuffers(1, &coord_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, coord_buffer);
  glBufferData(GL_ARRAY_BUFFER, 2 * grid->VertexCount() * sizeof(float), grid->GetCoords(), GL_STATIC_DRAW);

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(1);

  //buffer com os indices do grid
  GLuint index_buffer;
  glGenBuffers(1, &index_buffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_nind * sizeof(unsigned int), grid->GetIndices(), GL_STATIC_DRAW);
}

Quad::~Quad () 
{
}

void Quad::Draw (StatePtr )
{
  glBindVertexArray(m_vao);
  //glVertexAttrib3f(1,0.0f,0.0f,1.0f); // constant for all vertices
  //glVertexAttrib3f(2,1.0f,0.0f,0.0f); // constant for all vertices
  glDrawElements(GL_TRIANGLES,m_nind,GL_UNSIGNED_INT,0);
}