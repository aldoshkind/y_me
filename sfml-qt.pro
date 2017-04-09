#-------------------------------------------------
#
# Project created by QtCreator 2017-03-24T08:57:36
#
#-------------------------------------------------

QT       += core gui positioning

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = sfml-qt
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp \
    QSFMLCanvas.cpp \
    tiler.cpp \
	tiler_google.cpp \
    texture_cache.cpp \
    map_view.cpp \
    tree/filepath_utils.cpp \
    tree/resource.cpp \
    tree/widget.cpp \
    route.cpp

HEADERS  += \
    QSFMLCanvas.h \
    tiler.h \
    tiler_google.h \
    texture_cache.h \
    projector.h \
    projector_spheric.h \
    projector_elliptic.h \
    map_view.h \
    additional_math.h \
    geo_point.h \
    main_widget.h \
    render_node.h \
    geo_object.h \
    geo_node.h \
    route.h \
    tree/event_printer.h \
    tree/filepath_utils.h \
    tree/node.h \
    tree/resource.h \
    tree/sinus_generator.h \
    tree/tree_node.h \
    tree/widget.h \
    yuneec/interest_point.h \
    yuneec/route.h \
    yuneec/waypoint.h \
    json_serializable.h

LIBS += -lsfml-window -lsfml-graphics -lcurl -lsfml-system -ljsoncpp
