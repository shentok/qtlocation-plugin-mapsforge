TEMPLATE = lib
CONFIG += plugin
TARGET = qtgeoservices_mapsforge
QT += location-private positioning-private xml

PLUGIN_TYPE = geoservices
PLUGIN_CLASS_NAME = QGeoServiceProviderFactoryOsm
#load(qt_plugin)

INCLUDEPATH += \
    libqmapsforge

LIBS += \
    -Llibqmapsforge/mapsforgereader \
    -Llibqmapsforge/osmarender \
    -Llibqmapsforge/mapsforgerenderer \
    -Wl,-whole-archive -lmapsforgereader -losmarender -lmapsforgerenderer -Wl,-no-whole-archive

HEADERS += \
    qgeoserviceproviderpluginosm.h \
    qgeotiledmappingmanagerengineosm.h \
    qgeotilefetcherosm.h \
    qgeomapreplyosm.h


SOURCES += \
    qgeoserviceproviderpluginosm.cpp \
    qgeotiledmappingmanagerengineosm.cpp \
    qgeotilefetcherosm.cpp \
    qgeomapreplyosm.cpp

OTHER_FILES += \
    mapsforge_plugin.json

