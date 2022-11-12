#ifndef S989_SNDNODE_H
#define S989_SNDNODE_H

#include "s2wcontext.h"
#include "seq/isequence.h"
#include "seq/itrack.h"
#include "buffernode.h"
#include "synth.h"
#include "loader.h"
#include "sound_handler.h"
#include <istream>
#include <string>
#include <memory>
class SndNode;

class SndSequence : public BaseSequence<BasicTrack>
{
public:
  static double loadDuration(S2WContext* s2w, const std::string& path);
  SndSequence(SynthContext* ctx);

  void load(const std::string& path);
  void load(std::istream& stream);
  double duration() const { return m_duration; }
  bool isFinished() const;

private:
  SynthContext* synthCtx;
  double m_duration;
  std::shared_ptr<SndNode> node;
};

class SndNode : public BufferNode {
public:
  SndNode(double maxTime, const SynthContext* ctx);

  void load(std::istream& stream);
  bool isFinished() const;

protected:
  virtual int fillBuffer(std::vector<int16_t>& buf);

private:
  snd::loader m_loader;
  snd::synth m_synth;
  std::unique_ptr<snd::sound_handler> handler;
  double maxTime;
  bool done;
};

#endif
