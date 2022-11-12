PLUGIN_NAME = 989snd2wav
PLUGIN_CXXFLAGS := -I$(ROOTPATH)/src/sndplay/src $(shell pkg-config --cflags fmt)
PLUGIN_LDFLAGS := $(shell pkg-config --libs fmt)
-include $(ROOTPATH)seq2wav/config.mak
