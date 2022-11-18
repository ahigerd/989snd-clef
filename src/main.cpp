#include "sndnode.h"
#include "riffwriter.h"
#include "synth/synthcontext.h"
#include "synth/channel.h"
#include "commandargs.h"
#include <fstream>
#include <iostream>
#include <iomanip>

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
  ctx.save(&riff);
  riff.close();
  return 0;
}


