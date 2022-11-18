isEmpty(BUILDPATH) {
  error("BUILDPATH must be set")
}
BUILDPATH = $$absolute_path($$BUILDPATH)
include($$BUILDPATH/../seq2wav/gui/gui.pri)

HEADERS += sndwidget.h
SOURCES += sndwidget.cpp

SOURCES += main.cpp ../plugins/s2wplugin.cpp
