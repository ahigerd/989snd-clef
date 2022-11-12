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
  SndSequence seq(&ctx, 100);
  seq.load(infile);
  std::string filename = args.getString("output", infile + ".wav");
  std::cerr << "Writing " << (int(ctx.maximumTime() * 10) * .1) << " seconds to \"" << filename << "\"..." << std::endl;
  RiffWriter riff(ctx.sampleRate, true);
  riff.open(filename);
  ctx.save(&riff);
  riff.close();
  return 0;
}


