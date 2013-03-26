#ifndef NoiseTreeAnalysisOptions_h_
#define NoiseTreeAnalysisOptions_h_

#include <iostream>

#include "CmdLine.hh"

//
// Class NoiseTreeAnalysisOptions must have
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
// NoiseTreeAnalysisOptions object is a "const" member in the analysis
// class, so it is safe to make NoiseTreeAnalysisOptions a struct.
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
struct NoiseTreeAnalysisOptions
{
    NoiseTreeAnalysisOptions()
        : maxLogContribution(10.0),
          nPhiBins(144),
          minTSlice(4),
          maxTSlice(6)
    {
    }

    void parse(CmdLine& cmdline)
    {
        cmdline.option(NULL, "--converters") >> convertersGSSAFile;
        cmdline.option(NULL, "--maxLogContribution") >> maxLogContribution;
        cmdline.option(NULL, "--nPhiBins") >> nPhiBins;
        cmdline.option(NULL, "--minTSlice") >> minTSlice;
        cmdline.option(NULL, "--maxTSlice") >> maxTSlice;

        if (minTSlice > 10 || maxTSlice > 10 || minTSlice >= maxTSlice)
            throw CmdLineError("Invalid specification for time slice integration");

        if (maxLogContribution < 0.0)
            throw CmdLineError("Invalid specification for maxLogContribution");
    }

    void usage(std::ostream& os) const
    {
        os << "[--converters converterFile]"
           << " [--maxLogContribution value]"
           << " [--nPhiBins nBins]"
           << " [--minTSlice tSlice]"
           << " [--maxTSlice tSlice]"
            ;
    }

    std::string convertersGSSAFile;
    double maxLogContribution;
    unsigned nPhiBins;
    unsigned minTSlice;
    unsigned maxTSlice;
};

std::ostream& operator<<(std::ostream& os, const NoiseTreeAnalysisOptions& o)
{
    os << "converters = \"" << o.convertersGSSAFile << '"'
       << ", maxLogContribution = " << o.maxLogContribution
       << ", nPhiBins = " << o.nPhiBins
       << ", minTSlice = " << o.minTSlice
       << ", maxTSlice = " << o.maxTSlice
        ;
    return os;
}

#endif // NoiseTreeAnalysisOptions_h_
