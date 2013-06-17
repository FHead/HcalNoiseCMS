#ifndef NoiseTreeAnalysisOptions_h_
#define NoiseTreeAnalysisOptions_h_

#include <iostream>

#include "CmdLine.hh"
#include "inputValidation.hh"

//
// Class NoiseTreeAnalysisOptions must have
//
// 1) Default constructor
//
// 2) Copy constructor (usually auto-generated)
//
// 3) Method "void parse(CmdLine& cmdline)"
//
// 4) Method "void listOptions(std::ostream& os) const" for printing
//    available options
//
// 5) Method "void usage(std::ostream& os) const" for printing usage
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
        : hbGeometryFile("Geometry/hb.ctr"),
          heGeometryFile("Geometry/he.ctr"),
          maxLogContribution(10.0),
          correctionPhaseNS(6.0),
          nPhiBins(144),
          minTSlice(4),
          maxTSlice(6),
          hpdShapeNumber(105)
    {
    }

    void parse(CmdLine& cmdline)
    {
        cmdline.option(NULL, "--converters") >> convertersGSSAFile;
        cmdline.option(NULL, "--hbgeo") >> hbGeometryFile;
        cmdline.option(NULL, "--hegeo") >> heGeometryFile;
        cmdline.option(NULL, "--maxLogContribution") >> maxLogContribution;
        cmdline.option(NULL, "--correctionPhaseNS") >> correctionPhaseNS;
        cmdline.option(NULL, "--nPhiBins") >> nPhiBins;
        cmdline.option(NULL, "--minTSlice") >> minTSlice;
        cmdline.option(NULL, "--maxTSlice") >> maxTSlice;
        cmdline.option(NULL, "--hpdShapeNumber") >> hpdShapeNumber;

        validateRangeLELT(minTSlice, "minTSlice", 0U, 9U);
        validateRangeLELT(maxTSlice, "maxTSlice", minTSlice+1U, 10U);

        if (maxLogContribution < 0.0)
            throw CmdLineError("Invalid specification for maxLogContribution");
    }

    void listOptions(std::ostream& os) const
    {
        os << "[--converters converterFile]"
           << " [--hbgeo filename]"
           << " [--hegeo filename]"
           << " [--maxLogContribution value]"
           << " [--correctionPhaseNS value]"
           << " [--nPhiBins nBins]"
           << " [--minTSlice tSlice]"
           << " [--maxTSlice tSlice]"
           << " [--hpdShapeNumber value]"
            ;
    }

    void usage(std::ostream& os) const
    {
        os << " --converters            Name of the \"Geners\" string archive which contains\n"
           << "                         the functions that convert observed energy into\n"
           << "                         p-values. This file should normally be produced by\n"
           << "                         the \"analyzeEChanNtuple\" executable.\n\n";
        os << " --hbgeo                 File containing HB geometry description. The default\n"
           << "                         value of this option is \"Geometry/hb.ctr\". If this\n"
           << "                         value is incorrect (i.e., if the program is run from\n"
           << "                         some directory other than the source directory),\n"
           << "                         correct value of this option must be provided.\n\n";
        os << " --hegeo                 File containing HE geometry description. The default\n"
           << "                         value of this option is \"Geometry/he.ctr\". If this\n"
           << "                         value is incorrect (i.e., if the program is run from\n"
           << "                         some directory other than the source directory),\n"
           << "                         correct value of this option must be provided.\n\n";
        os << " --maxLogContribution    Maximum contribution (by modulus) a channel can make\n"
           << "                         into the energy-based pseudo loglikelihood of a group\n"
           << "                         of channels. Default value of this option is 10.0.\n\n";
        os << " --correctionPhaseNS     The value, in nanoseconds, of the \"phase\" parameter\n"
           << "                         for the energy pulse shape correction. Default value\n"
           << "                         of this option is 6.0.\n\n";
        os << " --nPhiBins              Number of bins to use for histograms of various phi\n"
           << "                         angle distributions. Default is 144.\n\n";
        os << " --minTSlice             Minimum time slice (included) for channel charge\n"
           << "                         determination. Default is 4.\n\n";
        os << " --maxTSlice             Maximum time slice (excluded) for channel charge\n"
           << "                         determination. Default is 6.\n\n";
        os << " --hpdShapeNumber        \"Pulse shape number\" for the energy pulse shape\n"
           << "                         correction. Default value of this option is 105.\n\n";
    }

    std::string convertersGSSAFile;
    std::string hbGeometryFile;
    std::string heGeometryFile;

    double maxLogContribution;
    double correctionPhaseNS;

    unsigned nPhiBins;
    unsigned minTSlice;
    unsigned maxTSlice;

    int hpdShapeNumber;
};

std::ostream& operator<<(std::ostream& os, const NoiseTreeAnalysisOptions& o)
{
    os << "converters = \"" << o.convertersGSSAFile << '"'
       << ", hbgeo = \"" << o.hbGeometryFile << '"'
       << ", hegeo = \"" << o.heGeometryFile << '"'
       << ", maxLogContribution = " << o.maxLogContribution
       << ", correctionPhaseNS = " << o.correctionPhaseNS
       << ", nPhiBins = " << o.nPhiBins
       << ", minTSlice = " << o.minTSlice
       << ", maxTSlice = " << o.maxTSlice
       << ", hpdShapeNumber = " << o.hpdShapeNumber
        ;
    return os;
}

#endif // NoiseTreeAnalysisOptions_h_
