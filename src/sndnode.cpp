#include "sndnode.h"
#include "s2wcontext.h"
#include "midi_handler.h"
#include "ame_handler.h"
#include "synth/synthcontext.h"
#include <iostream>
#include <stdexcept>

SndSequence::SndSequence(SynthContext* ctx, double d)
: BaseSequence(ctx->s2wContext()), synthCtx(ctx), m_duration(d)
{
  // initializers only
}

void SndSequence::load(const std::string& path)
{
  auto file = synthCtx->s2wContext()->openFile(path);
  load(*file);
}

void SndSequence::load(std::istream& stream)
{
  if (node) {
    throw std::runtime_error("already loaded a sequence");
  }
  node.reset(new SndNode(m_duration, synthCtx));
  node->load(stream);
  BasicTrack* track = new BasicTrack;
  AudioNodeEvent* event = new AudioNodeEvent(node);
  event->timestamp = 0;
  event->duration = m_duration;
  /*
  double fade = xsf.GetFadeMS(0) / 1000.0;
  if (fade > 0) {
    event->setEnvelope(0, 0, 1.0, fade);
  }
  */
  track->addEvent(event);
  /*
  if (fade > 0) {
    track->addEvent(new KillEvent(event->playbackID, event->duration));
    KillEvent* kill = new KillEvent(event->playbackID, event->duration + fade);
    kill->immediate = true;
    track->addEvent(kill);
  }
  */
  addTrack(track);
  synthCtx->addChannel(track);
}

bool SndSequence::isFinished() const
{
  if (!node) {
    return true;
  }
  if (node->isFinished()) {
    return true;
  }
  return BaseSequence::isFinished();
}

SndNode::SndNode(double maxTime, const SynthContext* ctx)
: BufferNode(maxTime, ctx), m_synth(m_loader), maxTime(maxTime), done(false)
{
  // initializers only
}

void SndNode::load(std::istream& stream)
{
  uint32_t bankId = m_loader.read_bank(stream);
  auto& bank = m_loader.get_bank(bankId);
  auto& sound = bank.sounds.at(0);
  auto header = m_loader.get_midi(sound.MIDIID);

  if (sound.Type == 4) {
    handler.reset(new snd::midi_handler(static_cast<snd::MIDIBlockHeader*>(header), m_synth, sound.Vol, 0, sound.Repeats, sound.VolGroup, m_loader));
  } else if (sound.Type == 5) {
    handler.reset(new snd::ame_handler(static_cast<snd::MultiMIDIBlockHeader*>(header), m_synth, sound.Vol, 0, sound.Repeats, sound.VolGroup, m_loader));
  } else {
    throw new std::runtime_error("unhandled sound type");
  }
}

int SndNode::fillBuffer(std::vector<int16_t>& buf)
{
  if (done) {
    return 0;
  }
  int numFrames = buf.size() / 400;
  int pos = 0;
  snd::s16_output sample;
  for (int i = 0; i < numFrames; i++) {
    for (int j = 0; j < 200; j++) {
      sample = m_synth.tick();
      buf[pos++] = sample.left;
      buf[pos++] = sample.right;
    }
    done = handler->tick();
    if (done) {
      return pos;
    }
  }
  return pos;
}

bool SndNode::isFinished() const
{
  return done;
}
