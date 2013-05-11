#ifndef MixedChargeAnalysisOptions_h_
#define MixedChargeAnalysisOptions_h_

#include <iostream>

#include "CmdLine.hh"

//
// Class MixedChargeAnalysisOptions must have
//
// 1) Default constructor
//
// 2) Copy constructor (usually auto-generated)
//
// 3) Method "void parse(CmdLine& cmdline)"
//
// 4) Method "void usage(std::ostream& os) const" for printing usage
//    instructions
//
// Preferably, this class should also have "operator<<" for printing
// the option values actually used.
//
// This class works in tandem with the analysis class.
// MixedChargeAnalysisOptions object is a "const" member in the analysis
// class, so it is safe to make MixedChargeAnalysisOptions a struct.
//
// The "parse" method must use normal methods of "CmdLine"
// ("option", "has", and "require") to fill the members of
// this class. Note that, if you find yourself using method
// "option" to assign values to some members, you should
// initialize these members in the default constructor.
//
// Do not use here switches reserved for use by the main program.
// These switches are:
//   "-h", "--histogram"
//   "-n", "--maxEvents"
//   "-s", "--noStats"
//   "-t", "--treeName"
//   "-v", "--verbose"
//
struct MixedChargeAnalysisOptions
{
    MixedChargeAnalysisOptions()
        : randomSeed(0UL),
          centralTS(4)
    {
    }

    void parse(CmdLine& cmdline)
    {
        cmdline.require("-c", "--configFile") >> objConfigFile;
        cmdline.require("-m", "--mixFile") >> mixListFile;
        cmdline.require("-r", "--randomSeed") >> randomSeed;

        cmdline.option(NULL, "--centralTS") >> centralTS;

        mixExtraChannels = cmdline.has("-e", "--mixExtra");
    }

    void usage(std::ostream& os) const
    {
        os << "-c configFile -m mixFile -r randomSeed "
           << "[-e] [--centralTS value]";
    }

    std::string objConfigFile;
    std::string mixListFile;
    unsigned long randomSeed;
    int centralTS;
    bool mixExtraChannels;
};

std::ostream& operator<<(std::ostream& os, const MixedChargeAnalysisOptions& o)
{
    os << "objConfigFile = \"" << o.objConfigFile << '"'
       << ", mixListFile = \"" << o.mixListFile << '"'
       << ", randomSeed = " << o.randomSeed
       << ", centralTS = " << o.centralTS
       << ", mixExtraChannels = " << o.mixExtraChannels
        ;
    return os;
}

#endif // MixedChargeAnalysisOptions_h_
