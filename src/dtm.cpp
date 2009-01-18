#include "dtm.h"

#define GL_GLEXT_PROTOTYPES

#include <QtOpenGL>
#include <QtCore>

DTM::DTM() {
}

DTM::~DTM() {
  delete[] m_vertices;
  delete[] m_indexes;
}

void DTM::load(QString filename) {
  QFile file(filename);

  if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    return;

  QTextStream in(&file);

  QString null;

  in >> null; in >> m_ncols;
  in >> null; in >> m_nrows;
  in >> null; in >> m_xllcorner;
  in >> null; in >> m_yllcorner;
  in >> null; in >> m_cellsize;
  in >> null; in >> m_nodata;

  m_nvertices = m_nrows*m_ncols;
  m_vertices = new float[3*m_nvertices];

  for(uint i=0; i<m_nrows; i++)
    for(uint j=0; j<m_ncols; j++) {
      const uint k = (i*m_ncols+j)*3;

      m_vertices[k] = i;
      m_vertices[k+1] = j;
      float z;
      in >> z;
      if(z == m_nodata) { // nodata
        if(i==0)
          m_vertices[k+2] = 0;
        else
          // on utilise simplement le point de la ligne précédente
          m_vertices[k+2] = m_vertices[((i-1)*m_ncols+j)*3+2];
      }
      else
        m_vertices[k+2] = z;
    }

  m_nindexes = 2*(m_nrows-1)*(m_ncols-1);
  m_indexes = new uint[3*m_nindexes];

  for(uint i=0; i<m_nrows-1; i++)
    for(uint j=0; j<m_ncols-1; j++) {
      const uint k = (i*(m_ncols-1)+j)*6 ;

      m_indexes[k] = i*m_ncols+j;
      m_indexes[k+1] = (i+1)*m_ncols+j;
      m_indexes[k+2] = i*m_ncols+j+1;

      m_indexes[k+3] = (i+1)*m_ncols+j+1;
      m_indexes[k+4] = (i+1)*m_ncols+j;
      m_indexes[k+5] = i*m_ncols+j+1;
    }

  file.close();
}

void DTM::initVBO() {
  glGenBuffers(2, m_buffers);

  glBindBuffer(GL_ARRAY_BUFFER, m_buffers[VERTICES]);
  glBufferData(GL_ARRAY_BUFFER, 3*m_nvertices*sizeof(float), m_vertices, GL_STATIC_DRAW);
  glVertexPointer(3, GL_FLOAT, 0, 0);
  glEnableClientState(GL_VERTEX_ARRAY);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffers[INDEXES]);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3*m_nindexes*sizeof(uint), m_indexes, GL_STATIC_DRAW);
}

void DTM::draw() const {
  glColor3f(1,1,1);

  //glPolygonMode(GL_FRONT,GL_LINE);
  //glPolygonMode(GL_BACK,GL_LINE);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffers[INDEXES]);
  glDrawElements(GL_TRIANGLES, 3*m_nindexes, GL_UNSIGNED_INT, 0);
}
