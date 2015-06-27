TEMPLATE = subdirs


SUBDIRS += \
    libqmapsforge \
    plugin \
    dummy

plugin.depends = libqmapsforge
plugin.file = plugin.pro

dummy.depends = plugin
dummy.file = dummy.pro

libqmapsforge.file = libqmapsforge/libqmapsforge.pro
