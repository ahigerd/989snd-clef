#include "buffernode.h"
#include "synth/synthcontext.h"
#include <iostream>

BufferNode::BufferNode(double maxTime, const SynthContext* ctx)
: AudioNode(ctx), buffer(int(ctx->sampleRate)), samplesAvailable(0), bufferTime(-1), maxTime(maxTime), skipSilence(false), done(false)
{
  // initializers only
}

bool BufferNode::isActive() const
{
  return !done;
}

int16_t BufferNode::generateSample(double time, int channel)
{
  if (maxTime >= 0 && time >= maxTime) {
    done = true;
    return 0;
  }
  uint32_t pos = (uint32_t((time - bufferTime) * ctx->sampleRate) << 1) + channel;
  if (pos >= samplesAvailable) {
    do {
      pos = 0;
      samplesAvailable = fillBuffer(buffer);
      if (samplesAvailable <= 0) {
        done = true;
        return 0;
      }
      while (skipSilence && pos < samplesAvailable) {
        if (buffer[pos] || buffer[pos + 1]) {
          skipSilence = false;
        } else {
          pos += 2;
        }
      }
    } while (skipSilence);
    bufferTime = time + (pos / ctx->sampleRate);
    pos += channel;
  }
  return buffer[pos];
}
