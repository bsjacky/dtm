INCLUDE_DIRECTORIES(../include ${FLOWVR_INCLUDE_DIR})

INCLUDE(${QT_USE_FILE})

SET(EXEC flood)
SET(SRC flood.cpp flowvr.cpp main.cpp)

SET(MOC_HDR flood.h)
QT4_WRAP_CPP(MOC_SRC ${MOC_HDR})

ADD_EXECUTABLE(${EXEC} ${SRC} ${MOC_SRC})
TARGET_LINK_LIBRARIES(${EXEC} ${FLOWVR_base_LIBRARY} ${FLOWVR_mod_LIBRARY} ${FLOWVR_utils_LIBRARY} ${QT_QTCORE_LIBRARY})
INSTALL(TARGETS ${EXEC} DESTINATION ${MODULE_DIR})
