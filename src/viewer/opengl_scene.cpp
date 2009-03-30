#include "opengl_scene.h"

#include "flowvr.h"
#include "camera.h"
#include "dtm.h"
#include "light.h"
#include "water.h"
#include "flowvr_thread.h"

#include <iostream>

OpenGLScene::OpenGLScene(QString fileName, QWidget *parent):
  QGLWidget(parent),
  m_fileName(fileName.remove(QRegExp(".grd$")))
{
  setWindowTitle("Flood simulation");

  m_current = 0;
  m_last = 0;

  m_refresh.setInterval(1000 / FPS);
  m_refresh.setSingleShot(true);
  connect(&m_refresh, SIGNAL(timeout()), this, SLOT(update()));
  m_refresh.start();

  m_ticks.start();
}

OpenGLScene::~OpenGLScene()
{
  delete m_camera;
  delete m_dtm;
  delete m_light;
  delete m_water;

  m_flowVRThread->quit();
  delete m_flowVRThread;
}

void OpenGLScene::initializeGL()
{
  m_camera = new Camera;
  m_dtm = new DTM(m_fileName);
  m_light = new Light(m_dtm);
  m_water = new Water(m_dtm);

  m_flowVRThread = new FlowVRThread(m_dtm, m_water);
  connect(m_flowVRThread, SIGNAL(updated()), m_water, SLOT(update()));
  m_flowVRThread->start();

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  QPixmap texture(m_fileName + ".png");
  if(texture.isNull()) {
    std::cerr << "Warning: error while reading texture file" << std::endl;
  }
  else {
    bindTexture(texture, GL_TEXTURE_2D);
    glEnable(GL_TEXTURE_2D);
  }

  m_camera->mouseMove(0, 0);
}

void OpenGLScene::resizeGL(const int w, const int h)
{
  glViewport(0, 0, w, h);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45, (float) w / (float) h, 1, 3000);
}

void OpenGLScene::paintGL()
{
  glClearColor(0, 0, 0, 0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  m_camera->look();

  m_dtm->draw();
  m_water->draw();
}

void OpenGLScene::mouseMoveEvent(QMouseEvent *e)
{
  if(e->buttons() == Qt::LeftButton) {
    QCursor::setPos(QPoint(mapToGlobal(QPoint(width() / 2, height() / 2))));

    int relx = e->globalX() - QCursor::pos().x();
    int rely = e->globalY() - QCursor::pos().y();

    m_camera->mouseMove(relx, rely);
  }
}

void OpenGLScene::mousePressEvent(QMouseEvent *e)
{
  if(e->button() == Qt::LeftButton) {
    setCursor(QCursor(Qt::BlankCursor));
    QCursor::setPos(QPoint(mapToGlobal(QPoint(width() / 2, height() / 2))));
  }
}

void OpenGLScene::mouseReleaseEvent(QMouseEvent *e)
{
  if(e->button() == Qt::LeftButton)
    setCursor(QCursor(Qt::ArrowCursor));
}

void OpenGLScene::wheelEvent(QWheelEvent *e)
{
  if(e->orientation() == Qt::Vertical)
    m_camera->wheel(e->delta() > 0);
}

void OpenGLScene::keyPressEvent(QKeyEvent *e)
{
  m_camera->keyPress(e->key(), true);
}

void OpenGLScene::keyReleaseEvent(QKeyEvent *e)
{
  m_camera->keyPress(e->key(), false);
}

void OpenGLScene::enterEvent(QEvent *)
{
  grabKeyboard();
}

void OpenGLScene::leaveEvent(QEvent *)
{
  m_camera->clearKeyStates();
  releaseKeyboard();
}

void OpenGLScene::update()
{
  m_current += m_ticks.restart();
  const uint elapsed = m_current - m_last;
  m_last = m_current;

  QTime ti;
  ti.start();

  m_camera->animate(elapsed);
  updateGL();

  const uint stop = m_current + ti.elapsed();
  if ((stop - m_last) < 1000 / FPS)
    m_refresh.setInterval(1000 / FPS - (stop - m_last));
  else
    m_refresh.setInterval(0);

  m_refresh.start();
}