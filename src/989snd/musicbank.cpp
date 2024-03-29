#include "musicbank.h"
#include <iostream>

#include "ame_handler.h"
#include "midi_handler.h"

namespace snd {

std::optional<std::unique_ptr<SoundHandler>> MusicBank::MakeHandler(VoiceManager& vm,
                                                                    u32 sound_id,
                                                                    s32 vol,
                                                                    s32 pan,
                                                                    s32 pm,
                                                                    s32 pb,
                                                                    u32 subsong) {
  auto& sound = Sounds[sound_id];

  // FIXME: global midi list
  // search only local bank for now
  // (always the case in jak games anyway)

  if (sound.Type == 4) {
    auto& midi = std::get<Midi>(MidiData);
    if (sound.MIDIID == midi.ID) {
      return std::make_unique<MidiHandler>(&midi, vm, sound, vol, pan, *this);
    }
    return std::nullopt;
  } else if (sound.Type == 5) {
    auto& midi = std::get<MultiMidi>(MidiData);
    if (sound.MIDIID == midi.ID) {
      return std::make_unique<AmeHandler>(&midi, vm, sound, vol, pan, *this, subsong);
    }
    return std::nullopt;
  } else {
    std::cerr << "Invalid music sound type" << std::endl;
    return std::nullopt;
    // error
  }
}

std::optional<std::unique_ptr<SoundHandler>> MusicBank::MakeHandler(VoiceManager& vm,
                                                                    u32 sound_id,
                                                                    s32 vol,
                                                                    s32 pan,
                                                                    SndPlayParams& params) {
  return std::nullopt;
}

}  // namespace snd
