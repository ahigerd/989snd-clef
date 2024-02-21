#include "codec/sampledata.h"
#include "sndnode.h"
#include "plugin/baseplugin.h"

// In the functions below, ctx->openFile() is provided by the plugin interface. Use
// this instead of standard library functions to open additional files in order to use
// the host's virtual filesystem.

struct ClefPluginInfo : public TagsM3UMixin {
  CLEF_PLUGIN_STATIC_FIELDS

  static bool isPlayable(ClefContext* /*ctx*/, const std::string& /*filename*/, std::istream& /*file*/) {
    // Implementations should check to see if the file is supported.
    // Return false or throw an exception to report failure.
    return true;
  }

  static int sampleRate(ClefContext* /*ctx*/, const std::string& /*filename*/, std::istream& /*file*/) {
    // Implementations should return the sample rate of the file.
    // This can be hard-coded if the plugin always uses the same sample rate.
    return 48000;
  }

  static double length(ClefContext* ctx, const std::string& filename, std::istream& /*file*/) {
    // Implementations should return the length of the file in seconds.
    return SndSequence::loadDuration(ctx, filename);
  }

  SynthContext* prepare(ClefContext* ctx, const std::string& filename, std::istream& file) {
    // Prepare to play the file. Load any necessary data into memory and store any
    // applicable state in members on this plugin object.

    // Be sure to call this to clear the sample cache:
    ctx->purgeSamples();

    ctx->pluginData = nullptr;
    SynthContext* synth = new SynthContext(ctx, 48000);
    SndSequence* seq = new SndSequence(synth);
    seq->load(file, SndSequence::splitParams(filename).second);
    ctx->pluginData = seq;

    return synth;
  }

  void release() {
    // Release any retained state allocated in prepare().
  }
};

const std::string ClefPluginInfo::version = "0.1.0";
const std::string ClefPluginInfo::pluginName = "989snd-clef";
const std::string ClefPluginInfo::pluginShortName = "989snd-clef";
ConstPairList ClefPluginInfo::extensions = { { "MUS", "989snd files (*.MUS)" } };
const std::string ClefPluginInfo::about =
  "989snd-clef copyright (C) 2022-2024 Adam Higerd\n"
  "Distributed under the MIT license.\n\n"
  "Based on jak-project, copyright (C) 2020-2022 OpenGOAL Team\n"
  "Used under the terms of the ISC license.";

CLEF_PLUGIN(ClefPluginInfo);
