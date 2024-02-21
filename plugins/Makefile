ROOTPATH := ../
include ../config.mak

GUI_OBJS = $(wildcard ../$(BUILDPATH)/gui/*.o)
OBJS_R = $(filter-out %_d.o $(GUI_OBJS) ../$(BUILDPATH)/main.o,$(wildcard ../$(BUILDPATH)/*.o ../$(BUILDPATH)/*/*.o ../$(BUILDPATH)/sndplay/src/*.o))
OBJS_D = $(filter-out $(GUI_OBJS) ../$(BUILDPATH)/main_d.o,$(wildcard ../$(BUILDPATH)/*_d.o ../$(BUILDPATH)/*/*_d.o ../$(BUILDPATH)/sndplay/src/*_d.o))

../aud_$(PLUGIN_NAME).$(DLL): $(OBJS_R) ../libclef/$(BUILDPATH)/libclef.a $(wildcard ../libclef/src/plugins/*.h) clefplugin.cpp Makefile
	$(CXX) -shared -o $@ $(CXXFLAGS_R) -DBUILD_AUDACIOUS clefplugin.cpp $(shell pkg-config --cflags --libs audacious) $(OBJS_R) $(LDFLAGS_R)
	strip $@

../aud_$(PLUGIN_NAME)_d.$(DLL): $(OBJS_D) ../libclef/$(BUILDPATH)/libclef_d.$(DLL) $(wildcard ../libclef/src/plugins/*.h) clefplugin.cpp Makefile
	$(CXX) -shared -o $@ $(CXXFLAGS_D) -DBUILD_AUDACIOUS clefplugin.cpp $(shell pkg-config --cflags --libs audacious) $(OBJS_D) $(LDFLAGS_D)

../in_$(PLUGIN_NAME).$(DLL): $(OBJS_R) ../libclef/$(BUILDPATH)/libclef.a $(wildcard ../libclef/src/plugins/*.h) clefplugin.cpp Makefile
	$(CXX) -shared -o $@ $(CXXFLAGS_R) -DBUILD_WINAMP clefplugin.cpp $(OBJS_R) $(LDFLAGS_R)
	strip $@

../in_$(PLUGIN_NAME)_d.$(DLL): $(OBJS_D) ../libclef/$(BUILDPATH)/libclef_d.$(DLL) $(wildcard ../libclef/src/plugins/*.h) clefplugin.cpp Makefile
	$(CXX) -shared -o $@ $(CXXFLAGS_D) -DBUILD_WINAMP clefplugin.cpp $(OBJS_D) $(LDFLAGS_D)

FORCE:
