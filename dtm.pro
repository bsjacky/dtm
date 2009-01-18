TEMPLATE = app
TARGET = 
DEPENDPATH += .
INCLUDEPATH += .

QT += opengl

HEADERS += src/camera.h \
           src/dtm.h \
           src/main_window.h \
           src/opengl_scene.h \
           src/point3d.h

SOURCES += src/camera.cpp \
           src/dtm.cpp \
           src/main.cpp \
           src/main_window.cpp \
           src/opengl_scene.cpp \
           src/point3d.cpp
