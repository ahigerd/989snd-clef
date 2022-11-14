#include "sndnode.h"
#include "s2wcontext.h"
#include "989snd/ame_handler.h"
#include "synth/synthcontext.h"
#include "tagmap.h"
#include <fstream>
#include <stdexcept>

SndSequence::SndSequence(SynthContext* ctx)
: BaseSequence(ctx->s2wContext()), synthCtx(ctx), m_duration(100)
{
  // initializers only
}

double SndSequence::loadDuration(S2WContext* s2w, const std::string& path)
{
  std::string tagsPath = TagsM3U::relativeTo(path);
  try {
    auto tagsFile = s2w->openFile(tagsPath);
    if (tagsFile && tagsFile->good()) {
      TagsM3U m3u(*tagsFile);
      int track = m3u.findTrack(path);
      std::string durationStr = m3u.get(track, "length_seconds_fp");
      if (durationStr.size()) {
        return std::stod(durationStr, nullptr);
      }
    }
  } catch (std::exception& e) {
    // fall through
    std::cerr << "Error reading tags: " << e.what() << std::endl;
  } catch (...) {
    // fall through
    std::cerr << "Error reading tags" << std::endl;
  }
  return 100;
}

void SndSequence::load(const std::string& path)
{
  m_duration = loadDuration(synthCtx->s2wContext(), path);

  auto file = synthCtx->s2wContext()->openFile(path);
  load(*file);
}

void SndSequence::load(std::istream& stream)
{
  if (node) {
    throw std::runtime_error("already loaded a sequence");
  }
  node.reset(new SndNode(m_duration, synthCtx));
  node->load(stream, 0);
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
: BufferNode(maxTime, ctx), m_vmanager(m_synth, m_loader), maxTime(maxTime), done(false)
{
  // initializers only
}

void SndNode::load(std::istream& stream, int subsong)
{
  uint32_t bankId = m_loader.read_bank(stream);
  if (bankId == 0xFFFFFFFF) {
    throw std::runtime_error("no supported content");
  }
  auto bank = m_loader.get_bank_by_handle(bankId);
  handler = bank->make_handler(m_vmanager, subsong, 0x400, 0, 0, 0);
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
