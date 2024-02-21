// Copyright: 2022 - 2024, Adam Higerd
// SPDX-License-Identifier: ISC
#pragma once

#include <vector>
#include <map>
#include <set>
#include <functional>

namespace snd {
struct sound_analyzer {
  static sound_analyzer* instance;

  enum { excite = 16 };

  enum operation {
    lte,
    lt,
    ne,
    gt,
    call,
    variant,
    set,
    inc,
    dec,
  };
  struct reg_range {
    u8 min;
    u8 max;
    u16 channels;

    inline bool operator==(const reg_range& other) const { return min == other.min && max == other.max; }
  };
  struct reg_behavior {
    operation op;
    u8 arg;

    inline bool operator==(const reg_behavior& other) const { return op == other.op && arg == other.arg; }
  };

  std::vector<reg_behavior> regs[17];
  std::map<int, std::vector<reg_range>> groups;
  std::function<void(int)> repeat_callback;

  static void log_reg_op(u8 index, operation op, u8 arg = 0);
  inline static void log_excite_op(operation op, u8 arg) { log_reg_op(excite, op, arg); }
  static void log_variant(int group, u8 min, u8 max, u16 channel_mask);
  static void log_repeat(int count);

  sound_analyzer();
  ~sound_analyzer();
};
}
