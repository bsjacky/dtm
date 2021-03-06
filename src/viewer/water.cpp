#include "water.h"

#include "dtm.h"
#include "point3d.h"

#define GL_GLEXT_PROTOTYPES

#include <QtOpenGL>

#include <algorithm>

Water::Water(const DTM *dtm):
  m_dtm(dtm),
  m_ncols(dtm->ncols()),
  m_nrows(dtm->nrows()),
  m_values(new float[m_nrows * m_ncols])
{
  for(index_t i = 0; i < m_nrows; ++i)
    for(index_t j = 0; j < m_ncols; ++j) {
      const index_t k = i * m_ncols + j;
      m_values[k] = 0;
    }

  initVertices();
  initIndices();
  initVBO();
  free();
}

Water::~Water()
{
  delete[] m_values;
  delete[] m_vertices;
}

void Water::initVertices()
{
  m_nvertices = m_nrows * m_ncols;
  m_vertices = new Point3d[m_nvertices];

  for(index_t i = 0; i < m_nrows; ++i)
    for(index_t j = 0; j < m_ncols; ++j) {
      const index_t k = i * m_ncols + j;

       m_vertices[k].setX(i);
       m_vertices[k].setY(j);
       m_vertices[k].setZ(m_dtm->minZ());
    }
}

void Water::initIndices()
{
  m_nindices = 3 * 2 * (m_nrows - 1) * (m_ncols - 1);
  m_indices = new index_t[m_nindices];

  for(index_t i=0; i < m_nrows - 1; ++i)
    for(index_t j=0; j<m_ncols-1; ++j) {
      const index_t k = 3 * 2 * (i * (m_ncols - 1) + j);

      // triangle 1
      m_indices[k] = i * m_ncols + j;
      m_indices[k + 1] = (i + 1) * m_ncols + j;
      m_indices[k + 2] = i * m_ncols + j + 1;

      // triangle 2
      m_indices[k + 3] = (i + 1) * m_ncols + j + 1;
      m_indices[k + 4] = (i + 1 ) * m_ncols + j;
      m_indices[k + 5] = i * m_ncols + j + 1;
    }
}

void Water::initVBO()
{
  glGenBuffers(2, m_buffers);

  glBindBuffer(GL_ARRAY_BUFFER, m_buffers[VERTICES]);
  glBufferData(GL_ARRAY_BUFFER, m_nvertices*sizeof(Point3d), m_vertices, GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffers[INDICES]);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_nindices*sizeof(index_t), m_indices, GL_STATIC_DRAW);
}

void Water::free()
{
  delete[] m_indices;
}

void Water::draw() const
{
  glBindBuffer(GL_ARRAY_BUFFER, m_buffers[VERTICES]);
  glVertexPointer(3, GL_FLOAT, 0, 0);
  glEnableClientState(GL_VERTEX_ARRAY);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffers[INDICES]);

  glColor4f(0, 0.34, 0.68, 0.4);
  glDrawElements(GL_TRIANGLES, m_nindices, GL_UNSIGNED_INT, 0);
  glColor4f(1, 1, 1, 1);

  glDisableClientState(GL_VERTEX_ARRAY);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Water::updateVBO()
{
  glBindBuffer(GL_ARRAY_BUFFER, m_buffers[VERTICES]);
  glBufferData(GL_ARRAY_BUFFER, m_nvertices * sizeof(Point3d), m_vertices, GL_DYNAMIC_DRAW);
}
