#include "sndnode.h"
#include "s2wcontext.h"
#include "989snd/ame_handler.h"
#include "synth/synthcontext.h"
#include "tagmap.h"
#include <fstream>
#include <stdexcept>

std::pair<std::string, std::string> SndSequence::splitParams(const std::string& path)
{
  int qPos = path.find("?");
  if (qPos == std::string::npos) {
    return std::make_pair(path, std::string());
  } else {
    std::string pathOnly = path.substr(0, qPos);
    std::string params = path.substr(qPos + 1);
    return std::make_pair(pathOnly, params);
  }
}

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
  auto split = splitParams(path);
  if (snd::sound_analyzer::instance) {
    m_duration = -1;
  } else {
    m_duration = loadDuration(synthCtx->s2wContext(), split.first);
  }
  auto file = synthCtx->s2wContext()->openFile(split.first);
  load(*file, split.second);
}

void SndSequence::load(std::istream& stream, const std::string& params)
{
  if (node) {
    throw std::runtime_error("already loaded a sequence");
  }
  node.reset(new SndNode(m_duration, synthCtx));
  auto p = loadParams(params);
  node->load(stream, p.subsong);
  for (const auto& reg : p.regs) {
    int regIndex = reg.first;
    if (regIndex >= 0 && regIndex <= 16) {
      int regValue = reg.second;
      setRegister(regIndex, regValue);
    }
  }
  SndTrack* track = new SndTrack(node);
  addTrack(track);
  synthCtx->addChannel(track);
}

SndSequence::LoadParams SndSequence::loadParams(const std::string& params)
{
  LoadParams result;
  result.subsong = 0;
  if (!params.size()) {
    return result;
  }
  int andPos = -1;
  try {
    do {
      int start = andPos + 1;
      andPos = params.find("&", start);
      std::string param = params.substr(start, andPos);
      int eqPos = param.find("=");
      if (eqPos == std::string::npos) {
        try {
          result.subsong = std::stoi(param);
        } catch (...) {
          result.subsong = 0;
        }
      } else {
        try {
          int regIndex = 16;
          if (param.substr(0, 0) != "x") {
            regIndex = std::stoi(param.substr(0, eqPos));
          }
          int regValue = std::stoi(param.substr(eqPos + 1));
          result.regs.emplace_back(regIndex, regValue);
        } catch (...) {
          // discard
        }
      }
    } while (andPos != std::string::npos);
  } catch (...) {
    // consume
  }
  return result;
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
  handler = bank->make_handler(m_vmanager, 0, 0x400, 0, 0, 0, subsong);
  done = handler->tick();
  this->subsong = subsong;
}

int SndNode::numSubsongs() const
{
  auto ame = dynamic_cast<snd::ame_handler*>(handler.get());
  if (ame) {
    return ame->num_subsongs();
  }
  return 0;
}

void SndNode::setSubsong(int index)
{
  if (numSubsongs() <= index) {
    done = true;
    return;
  }
  subsong = index;
  auto ame = dynamic_cast<snd::ame_handler*>(handler.get());
  if (ame) {
    ame->set_subsong(index);
  }
}

void SndNode::setRegister(int index, int value)
{
  if (index > 16) index = 16;
  regs[index] = value;
  handler->set_register(index, value);
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

void SndNode::stop()
{
  if (handler) {
    handler->stop();
  }
  done = true;
}

SndTrack::SndTrack(std::shared_ptr<SndNode> node)
: node(node), didReset(true)
{
  // initializers only
}

std::shared_ptr<SequenceEvent> SndTrack::readNextEvent()
{
  if (!didReset) {
    return nullptr;
  }
  AudioNodeEvent* event = new AudioNodeEvent(node);
  event->timestamp = 0;
  if (snd::sound_analyzer::instance) {
    event->duration = -1;
  } else {
    event->duration = node->duration();
  }
  /*
  double fade = xsf.GetFadeMS(0) / 1000.0;
  if (fade > 0) {
    event->setEnvelope(0, 0, 1.0, fade);
    track->addEvent(new KillEvent(event->playbackID, event->duration));
    KillEvent* kill = new KillEvent(event->playbackID, event->duration + fade);
    kill->immediate = true;
    track->addEvent(kill);
  }
  */
  didReset = false;
  return std::shared_ptr<SequenceEvent>(event);
}

void SndTrack::internalReset()
{
  didReset = true;
  node->setSubsong(node->getSubsong());
}

bool SndTrack::isFinished() const
{
  return !didReset;
}

double SndTrack::length() const
{
  return node->duration();
}

void SndTrack::seek(double timestamp)
{
  reset();
  seekEvent = std::shared_ptr<SequenceEvent>();
  lastEvent = std::shared_ptr<SequenceEvent>();
  ITrack::seek(timestamp);
}
