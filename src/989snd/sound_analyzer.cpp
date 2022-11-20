// Copyright: 2021 - 2022, Adam Higerd <chighland@gmail.com>
// SPDX-License-Identifier: ISC
#include "sound_handler.h"

namespace snd {

sound_analyzer* sound_analyzer::instance = nullptr;

sound_analyzer::sound_analyzer()
{
  if (instance == nullptr) {
    instance = this;
  }
}

sound_analyzer::~sound_analyzer()
{
  if (instance == this) {
    instance = nullptr;
  }
}

void sound_analyzer::log_reg_op(u8 index, operation op, u8 arg)
{
  if (!instance || index > 16) return;
  reg_behavior rb{ op, arg };
  for (const auto& b : instance->regs[index]) {
    if (b == rb) return;
  }
  instance->regs[index].push_back(rb);
}

void sound_analyzer::log_variant(int group, u8 min, u8 max, u16 channel_mask)
{
  if (!instance) return;
  reg_range rr{ min, max, channel_mask };
  for (auto& r : instance->groups[group]) {
    if (r == rr) {
      r.channels |= channel_mask;
      return;
    }
  }
  instance->groups[group].push_back(rr);
}

void sound_analyzer::log_repeat(int count)
{
  if (!instance || !instance->repeat_callback) return;
  instance->repeat_callback(count);
}

}
