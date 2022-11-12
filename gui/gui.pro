isEmpty(BUILDPATH) {
  error("BUILDPATH must be set")
}
BUILDPATH = $$absolute_path($$BUILDPATH)
include($$BUILDPATH/../seq2wav/gui/gui.pri)
INCLUDEPATH += $$PROJPATH/src/sndplay/src

SOURCES += main.cpp ../plugins/s2wplugin.cpp
