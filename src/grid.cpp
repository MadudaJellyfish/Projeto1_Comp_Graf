#include "grid.h"

#include <cassert>
#include <iostream>

#define INDEX(i,j,nx) ((unsigned int)((j)*(nx+1)+(i)))

GridPtr Grid::Make (int nx, int ny, float dis)
{
  return GridPtr(new Grid(nx,ny,dis));
}

Grid::Grid (int nx, int ny, float dis)
: m_nx(nx), m_ny(ny)
{
  // allocate and fill coordinates
  m_coords = new float[2*VertexCount()];
  float dx = dis / nx;
  float dy = dis / ny;
  int nc = 0;
  for (int j=0; j<=m_ny; ++j) {
    for (int i=0; i<=m_nx; ++i) {
      m_coords[nc++] = i*dx - 1.0f;
      m_coords[nc++] = j*dy - 1.0f;
    }
  }
  // allocate and fill indices
  m_indices = new unsigned int[IndexCount()];
  int ni = 0;
  for (int j=0; j<ny; ++j) {
    for (int i=0; i<nx; ++i) {
      m_indices[ni++] = INDEX(i,j,nx);
      m_indices[ni++] = INDEX(i+1,j,nx);
      m_indices[ni++] = INDEX(i+1,j+1,nx);
      m_indices[ni++] = INDEX(i,j,nx);
      m_indices[ni++] = INDEX(i+1,j+1,nx);
      m_indices[ni++] = INDEX(i,j+1,nx);
    }
  }
}

Grid::~Grid () 
{
  delete m_coords;
  delete m_indices;
}

int Grid::GetNx ()
{
  return m_nx;
}

int Grid::GetNy ()
{
  return m_ny;
}

int Grid::VertexCount ()
{
  return (m_nx+1)*(m_ny+1);
}

int Grid::IndexCount ()
{
  return 6*m_nx*m_ny;
}

const float* Grid::GetCoords () const
{
  return m_coords;
}

const unsigned int* Grid::GetIndices () const
{
  return m_indices;
}
