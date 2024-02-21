PLUGIN_NAME = 989snd-clef
-include $(ROOTPATH)libclef/config.mak
CXXFLAGS := $(subst -std=gnu++17,-std=gnu++2a,$(CXXFLAGS))
CXXFLAGS_R := $(subst -std=gnu++17,-std=gnu++2a,$(CXXFLAGS_R))
CXXFLAGS_D := $(subst -std=gnu++17,-std=gnu++2a,$(CXXFLAGS_D))
