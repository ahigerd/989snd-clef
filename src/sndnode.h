#ifndef S989_SNDNODE_H
#define S989_SNDNODE_H

#include "clefcontext.h"
#include "seq/isequence.h"
#include "seq/itrack.h"
#include "buffernode.h"
#include "989snd/midi_handler.h"
#include "989snd/synth.h"
#include "989snd/loader.h"
#include "989snd/sound_handler.h"
#include "989snd/musicbank.h"
#include "989snd/vagvoice.h"
#include <istream>
#include <string>
#include <memory>
#include <utility>

class SndNode : public BufferNode {
public:
  SndNode(double maxTime, const SynthContext* ctx);

  void load(std::istream& stream, int subsong);
  int numSubsongs() const;
  void setSubsong(int index);
  void setRegister(int index, int value);
  inline int getSubsong() const { return subsong; }
  inline int getRegister(int index) const { return regs[index]; }
  bool isFinished() const;
  void stop();

  inline double duration() const { return maxTime; }

protected:
  virtual int fillBuffer(std::vector<int16_t>& buf);

private:
  snd::loader m_loader;
  snd::synth m_synth;
  snd::voice_manager m_vmanager;
  std::unique_ptr<snd::sound_handler> handler;
  double maxTime;
  bool done;

  int subsong;
  int regs[17];
};

class SndTrack : public ITrack
{
public:
  SndTrack(std::shared_ptr<SndNode> node);

  virtual bool isFinished() const;
  virtual double length() const;
  virtual void seek(double timestamp);

protected:
  virtual std::shared_ptr<SequenceEvent> readNextEvent();
  virtual void internalReset();

private:
  std::shared_ptr<SndNode> node;
  bool didReset;
};

class SndSequence : public BaseSequence<SndTrack>
{
public:
  static double loadDuration(ClefContext* clef, const std::string& path);
  static std::pair<std::string, std::string> splitParams(const std::string& path);
  struct LoadParams {
    int subsong;
    std::vector<std::pair<int,int>> regs;
  };

  SndSequence(SynthContext* ctx);

  void load(const std::string& path);
  void load(std::istream& stream, const std::string& params = std::string());
  LoadParams loadParams(const std::string& params);
  inline int numSubsongs() const { if (!node) return 0; return node->numSubsongs(); }
  inline void setSubsong(int index) { if (!node) return; node->setSubsong(index); }
  inline void setRegister(int index, int value) { if (!node) return; node->setRegister(index, value); }
  inline int subsong() const { return node ? node->getSubsong() : 0; }
  inline int excite() const { return getRegister(16); }
  inline int getRegister(int index) const { return node ? node->getRegister(index) : 0; }

  double duration() const { return m_duration; }
  bool isFinished() const;

  inline void stop() { if (node) node->stop(); }

private:
  SynthContext* synthCtx;
  double m_duration;
  std::shared_ptr<SndNode> node;
};

#endif
