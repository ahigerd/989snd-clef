#ifndef SSF2WAV_BUFFERNODE_H
#define SSF2WAV_BUFFERNODE_H

#include "synth/audionode.h"
#include <vector>

class BufferNode : public AudioNode {
public:
  BufferNode(double maxTime, const SynthContext* ctx);

  virtual bool isActive() const;
  virtual int16_t generateSample(double time, int channel = 0);

protected:
  virtual int fillBuffer(std::vector<int16_t>& buf) = 0;

private:
  std::vector<int16_t> buffer;
  int samplesAvailable;
  double bufferTime, maxTime;
  bool skipSilence, done;
};

#endif
