SOURCES	+= \
		src/db_ops.cpp \
		src/db_query.cpp \
		src/db_insert.cpp \
		src/db_update.cpp \
		src/db_append.cpp \
		src/db_delete.cpp \

HEADERS	+= \
		include/db_ops.h \
		include/db_query.h \
		include/db_insert.h \
		include/db_update.h \
		include/db_append.h \
		include/db_delete.h \

FORMS	+= \


TEMPLATE	= lib
LANGUAGE	= C++

TARGET = sqlapps_rv1.0
DESTDIR        = bin
OBJECTS_DIR    = obj
MOC_DIR        = moc
UI_HEADERS_DIR = uic/include
UI_SOURCES_DIR = uic/src
RCC_DIR        = rcc
CONFIG	+= qt_release dynamic warn_on debug plugin thread exceptions rtti stl
QT += sql
DEPENDPATH += include
DEFINES +=
QMAKE_LIBDIR += /data0/ncpv/lib/
LIBS	+=
INCLUDEPATH += /usr/X11R6/include include /data0/ncpv/include /data0/lib/postgresql/postgresql/include


target.path += /data0/ncpv/lib
INSTALLS += target

