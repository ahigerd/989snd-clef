#include "sndnode.h"
#include "riffwriter.h"
#include "synth/synthcontext.h"
#include "synth/channel.h"
#include "989snd/sound_handler.h"
#include "commandargs.h"
#include <fstream>
#include <iostream>
#include <iomanip>

static const char* op_labels[] = {
  "greater than",
  "greater than or equal to",
  "equal to",
  "less than or equal to",
  "call",
  "select variant",
  "set value to",
  "increment value",
  "decrement value",
};

int main(int argc, char** argv)
{
  CommandArgs args({
    { "help", "h", "", "Show this help text" },
    { "output", "o", "filename", "Set the output filename (default: input filename with .wav extension)" },
    { "r0", "", "value", "Start with register 0 set to the specified value (default: 0)" },
    { "r1", "", "value", "Start with register 1 set to the specified value (default: 0)" },
    { "r2", "", "value", "Start with register 2 set to the specified value (default: 0)" },
    { "r3", "", "value", "Start with register 3 set to the specified value (default: 0)" },
    { "r4", "", "value", "Start with register 4 set to the specified value (default: 0)" },
    { "r5", "", "value", "Start with register 5 set to the specified value (default: 0)" },
    { "r6", "", "value", "Start with register 6 set to the specified value (default: 0)" },
    { "r7", "", "value", "Start with register 7 set to the specified value (default: 0)" },
    { "r8", "", "value", "Start with register 8 set to the specified value (default: 0)" },
    { "r9", "", "value", "Start with register 9 set to the specified value (default: 0)" },
    { "r10", "", "value", "Start with register 10 set to the specified value (default: 0)" },
    { "r11", "", "value", "Start with register 11 set to the specified value (default: 0)" },
    { "r12", "", "value", "Start with register 12 set to the specified value (default: 0)" },
    { "r13", "", "value", "Start with register 13 set to the specified value (default: 0)" },
    { "r14", "", "value", "Start with register 14 set to the specified value (default: 0)" },
    { "r15", "", "value", "Start with register 15 set to the specified value (default: 0)" },
    { "excite", "x", "value", "Start with the excitement value set to the specified value (default: 0)" },
    { "subsong", "s", "index", "Play selected subsong (default: 0)" },
    { "analyze", "a", "", "Analyze track for variants and length" },
    { "", "", "input", "Path to a .MUS file" },
  });
  std::string argError = args.parse(argc, argv);
  if (!argError.empty()) {
    std::cerr << argError << std::endl;
    return 1;
  } else if (args.hasKey("help") || argc < 2) {
    std::cout << args.usageText(argv[0]) << std::endl;
    return 0;
  } else if (args.positional().size() != 1) {
    std::cerr << argv[0] << ": exactly one input filename required" << std::endl;
    return 1;
  }

  std::string infile = args.positional().at(0);

  std::unique_ptr<snd::sound_analyzer> analyzer;
  if (args.hasKey("analyze")) {
    analyzer.reset(new snd::sound_analyzer());
  }

  S2WContext s2w;
  SynthContext ctx(&s2w, 48000);
  SndSequence seq(&ctx);
  seq.load(infile);
  if (args.hasKey("subsong")) {
    seq.setSubsong(args.getInt("subsong"));
  }
  for (int i = 0; i <= 15; i++) {
    std::string key = "r" + std::to_string(i);
    if (args.hasKey(key)) {
      seq.setRegister(i, args.getInt(key));
    }
  }
  if (args.hasKey("excite")) {
    seq.setRegister(16, args.getInt("excite"));
  }
  std::string filename = args.getString("output", infile + ".wav");
  std::cerr << "Writing " << (int(ctx.maximumTime() * 10) * .1) << " seconds to \"" << filename << "\"..." << std::endl;
  RiffWriter riff(ctx.sampleRate, true);
  riff.open(filename);

  double repeatDuration = -1;
  int repeatCount = -1;
  if (analyzer) {
    analyzer->repeat_callback = [&](int count) {
      repeatCount = count;
      repeatDuration = ctx.currentTime();
      seq.stop();
    };
  }

  ctx.save(&riff);
  riff.close();

  if (analyzer) {
    std::cerr << std::endl;
    if (repeatDuration < 0) {
      std::cerr << "Unable to analyze." << std::endl;
      return 1;
    }
    std::cerr << "Duration: " << repeatDuration << std::endl;
    if (repeatCount == 0) {
      std::cerr << "Loops infinitely" << std::endl;
    } else if (repeatCount >= 0) {
      std::cerr << "Loops x" << repeatCount << std::endl;
    }
    std::cerr << std::endl;
    for (int r = 0; r < 17; r++) {
      const auto& ops = analyzer->regs[r];
      if (!ops.size()) {
        continue;
      }
      if (r == snd::sound_analyzer::excite) {
        std::cerr << "Excite:" << std::endl;
      } else {
        std::cerr << "Register " << r << ":" << std::endl;
      }
      for (const auto& op : ops) {
        std::cerr << "\t" << op_labels[op.op] << " ";
        if (op.op != snd::sound_analyzer::call &&
            op.op != snd::sound_analyzer::inc &&
            op.op != snd::sound_analyzer::dec /* &&
            op.op != snd::sound_analyzer::variant */) {
          std::cerr << int(op.arg);
        }
        std::cerr << std::endl;
      }
      std::cerr << std::endl;
    }
    for (const auto& iter : analyzer->groups) {
      std::cerr << "Group " << iter.first << ":" << std::endl;
      int numChannels = 0;
      for (const auto& r : iter.second) {
        int ch = 0;
        uint16_t mask = r.channels;
        while (mask) {
          mask >>= 1;
          ch++;
        }
        if (ch > numChannels) numChannels = ch;
      }
      for (const auto& r : iter.second) {
        std::cerr << "\t" << int(r.min) << " - " << int(r.max) << ": ";
        for (int i = 0; i < numChannels; i++) {
          std::cerr << ((r.channels & (1 << i)) ? "+" : "-");
        }
        std::cerr << std::endl;
      }
      std::cerr << std::endl;
    }
  }

  return 0;
}


