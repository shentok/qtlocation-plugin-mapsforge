TEMPLATE = subdirs


SUBDIRS += \
    libqmapsforge \
    plugin

plugin.depends = libqmapsforge
plugin.file = plugin.pro

libqmapsforge.file = libqmapsforge/libqmapsforge.pro
