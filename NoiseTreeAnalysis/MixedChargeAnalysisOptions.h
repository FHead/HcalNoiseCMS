#ifndef MixedChargeAnalysisOptions_h_
#define MixedChargeAnalysisOptions_h_

#include <iostream>

#include "CmdLine.hh"
#include "inputValidation.hh"

//
// Class MixedChargeAnalysisOptions must have
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
        : hbGeometryFile("Geometry/hb.ctr"),
          heGeometryFile("Geometry/he.ctr"),
          channelSelector("LeadingJetChannelSelector"),
          pattRecoScale(0.2),
          etaToPhiBandwidthRatio(1.0),
          coneSize(0.5),
          peakEtCutoff(5.0),
          jetPtCutoff(20.0),
          chargeScaleFactor(1.0),
          minRecHitTime(-1.0e30),
          maxRecHitTime(1.0e30),
          randomSeed(0UL),
          centralTS(4),
          minResponseTS(4),
          maxResponseTS(6),
          minPreTS(2),
          maxPreTS(4),
          minPostTS(6),
          maxPostTS(8)
    {
    }

    void parse(CmdLine& cmdline)
    {
        mixExtraChannels = cmdline.has("-e", "--mixExtra");
        disableChargeMixing = cmdline.has(NULL, "--disableChargeMixing");

        if (disableChargeMixing)
        {
            cmdline.option("-c", "--configFile") >> objConfigFile;
            cmdline.option("-m", "--mixFile") >> mixListFile;
        }
        else
        {
            cmdline.require("-c", "--configFile") >> objConfigFile;
            cmdline.require("-m", "--mixFile") >> mixListFile;
        }

        cmdline.option(NULL, "--hbgeo") >> hbGeometryFile;
        cmdline.option(NULL, "--hegeo") >> heGeometryFile;
        cmdline.option(NULL, "--channelSelector") >> channelSelector;

        cmdline.option(NULL, "--pattRecoScale") >> pattRecoScale;
        cmdline.option(NULL, "--etaToPhiBandwidthRatio") >> etaToPhiBandwidthRatio;
        cmdline.option(NULL, "--coneSize") >> coneSize;
        cmdline.option(NULL, "--peakEtCutoff") >> peakEtCutoff;
        cmdline.option(NULL, "--jetPtCutoff") >> jetPtCutoff;
        cmdline.option(NULL, "--chargeScaleFactor") >> chargeScaleFactor;
        cmdline.option(NULL, "--minRecHitTime") >> minRecHitTime;
        cmdline.option(NULL, "--maxRecHitTime") >> maxRecHitTime;

        cmdline.option("-r", "--randomSeed") >> randomSeed;
        cmdline.option(NULL, "--filterFile") >> filterFile;
        cmdline.option(NULL, "--channelArchive") >> channelArchive;
        cmdline.option(NULL, "--centralTS") >> centralTS;
        cmdline.option(NULL, "--minResponseTS") >> minResponseTS;
        cmdline.option(NULL, "--maxResponseTS") >> maxResponseTS;
        cmdline.option(NULL, "--minPreTS") >> minPreTS;
        cmdline.option(NULL, "--maxPreTS") >> maxPreTS;
        cmdline.option(NULL, "--minPostTS") >> minPostTS;
        cmdline.option(NULL, "--maxPostTS") >> maxPostTS;

        validateRangeLELT(minResponseTS, "minResponseTS", 0U, 9U);
        validateRangeLELT(maxResponseTS, "maxResponseTS", minResponseTS+1U, 10U);
    }

    void listOptions(std::ostream& os) const
    {
        os << "-c configFile -m mixFile [-r randomSeed] [-e]"
           << " [--disableChargeMixing]"
           << " [--hbgeo filename]"
           << " [--hegeo filename]"
           << " [--channelSelector classname]"
           << " [--pattRecoScale value]"
           << " [--etaToPhiBandwidthRatio value]"
           << " [--coneSize value]"
           << " [--peakEtCutoff value]"
           << " [--jetPtCutoff value]"
           << " [--chargeScaleFactor value]"
           << " [--minRecHitTime value]"
           << " [--maxRecHitTime value]"
           << " [--filterFile filename]"
           << " [--channelArchive archiveName]"
           << " [--centralTS value]"
           << " [--minResponseTS value]"
           << " [--maxResponseTS value]"
           << " [--minPreTS value]"
           << " [--maxPreTS value]"
           << " [--minPostTS value]"
           << " [--maxPostTS value]"
            ;
    }

    void usage(std::ostream& os) const
    {
        os << " -c    Required option. Provide the configuration file for charge\n"
           << "       mixing (time shifts, number of events to mix, etc). This file\n"
           << "       should be generated by the python script \"make_configFile.py\".\n\n";
        os << " -m    Required option. This is just a text file which contains the\n"
           << "       list of files to be used as a source for charge mixing, one\n"
           << "       file name per line.\n\n";
        os << " -r    Specify the seed for the random number generator (for reproducibility).\n"
           << "       The default value of 0 means that the seed will be read from\n"
           << "       /dev/urandom (not reproducible).\n\n";
        os << " -e    Mix extra channels: not just channels read out in this event but all\n"
           << "       channels for which additional mixing was generated.\n\n";
        os << " --hbgeo             File containing HB geometry description. The default\n"
           << "                     value of this option is \"Geometry/hb.ctr\". If this\n"
           << "                     value is incorrect (i.e., if the program is run from\n"
           << "                     some directory other than the source directory),\n"
           << "                     correct value of this option must be provided.\n\n";
        os << " --hegeo             File containing HE geometry description. The default\n"
           << "                     value of this option is \"Geometry/he.ctr\". If this\n"
           << "                     value is incorrect (i.e., if the program is run from\n"
           << "                     some directory other than the source directory),\n"
           << "                     correct value of this option must be provided.\n\n";
        os << " --filterFile        The binary file with a vector of HcalChargeFilter\n"
           << "                     objects used for charge reconstruction from mixed data.\n"
           << "                     This file can be generated by the \"buildOptimalFilters\"\n"
           << "                     program. By default, this reconstruction is not performed.\n\n";
        os << " --channelArchive    The \"Geners\" archive into which the channel charge data\n"
           << "                     will be written for subsequent filter fitting by the\n"
           << "                     \"buildOptimalFilters\" program.  By default, no such\n"
           << "                     archive is created.\n\n";
        os << " --channelSelector   Class to use for selecting good channels. Valid\n"
              "                     values of this option are \"FFTJetChannelSelector\",\n"
              "                     \"LeadingJetChannelSelector\", and \"AllChannelSelector\".\n"
              "                     Default is \"LeadingJetChannelSelector\".\n\n";
        os << " --pattRecoScale     Pattern recognition scale for FFTJet jet reconstruction.\n"
           << "                     Default value is 0.2.\n\n";
        os << " --etaToPhiBandwidthRatio   Eta/phi pattern recognition bandwidth ratio and\n"
           << "                            cone axis ratio for FFTJet. Default value is 1.0.\n\n";
        os << " --coneSize          Geometric mean of eta-phi cone axes for jet\n"
           << "                     reconstruction. Default is 0.5.\n\n";
        os << " --peakEtCutoff      Peak magnitude cutoff (local Et) for jet reconstruction.\n"
           << "                     Default is 5.0.\n\n";
        os << " --jetPtCutoff       Minimum transverse momentum for \"good\" jets. Default\n"
           << "                     value is 20.0.\n\n";
        os << " --chargeScaleFactor Charge scale factor for mixed events. Default is 1.0.\n\n";
        os << " --minRecHitTime     Minimum RecHitTime for \"good\" channels. Default is\n"
           << "                     a negative number of large magnitude (all channels pass).\n\n";
        os << " --maxRecHitTime     Maximum RecHitTime for \"good\" channels. Default is\n"
           << "                     a large positive number (all channels pass).\n\n";
        os << " --centralTS         The \"central\" time slice (default is 4). The program\n"
           << "                     will check that the generated time shifts will not move\n"
           << "                     the central TS outside of observable range.\n\n";
        os << " --minResponseTS     Minimum time slice (included) for defining the \"real\"\n"
           << "                     signal charge before mixing.\n\n";
        os << " --maxResponseTS     Maximum time slice (excluded) for defining the \"real\"\n"
           << "                     signal charge before mixing.\n\n";
        os << " --minPreTS          Minimum time slice (included) for defining \"pre charge\"\n"
           << "                     before and after mixing.\n\n";
        os << " --maxPreTS          Maximum time slice (excluded) for defining \"pre charge\"\n"
           << "                     before and after mixing.\n\n";
        os << " --minPostTS         Minimum time slice (included) for defining \"post charge\"\n"
           << "                     before and after mixing.\n\n";
        os << " --maxPostTS         Maximum time slice (excluded) for defining \"post charge\"\n"
           << "                     before and after mixing.\n\n";
        os << " --disableChargeMixing   Disable all code related to charge mixing. This option\n"
           << "                         can be useful for testing purposes.\n\n";
    }

    std::string hbGeometryFile;
    std::string heGeometryFile;
    std::string objConfigFile;
    std::string mixListFile;
    std::string filterFile;
    std::string channelArchive;
    std::string channelSelector;

    double pattRecoScale;
    double etaToPhiBandwidthRatio;
    double coneSize;
    double peakEtCutoff;
    double jetPtCutoff;
    double chargeScaleFactor;
    double minRecHitTime;
    double maxRecHitTime;

    unsigned long randomSeed;
    int centralTS;
    unsigned minResponseTS;
    unsigned maxResponseTS;
    unsigned minPreTS;
    unsigned maxPreTS;
    unsigned minPostTS;
    unsigned maxPostTS;
    bool mixExtraChannels;
    bool disableChargeMixing;
};

std::ostream& operator<<(std::ostream& os, const MixedChargeAnalysisOptions& o)
{
    os << "configFile = \"" << o.objConfigFile << '"'
       << ", hbgeo = \"" << o.hbGeometryFile << '"'
       << ", hegeo = \"" << o.heGeometryFile << '"'
       << ", mixFile = \"" << o.mixListFile << '"'
       << ", filterFile = \"" << o.filterFile << '"'
       << ", channelArchive = \"" << o.channelArchive << '"'
       << ", channelSelector = \"" << o.channelSelector << '"'
       << ", pattRecoScale = \"" << o.pattRecoScale << '"'
       << ", etaToPhiBandwidthRatio = \"" << o.etaToPhiBandwidthRatio << '"'
       << ", coneSize = \"" << o.coneSize << '"'
       << ", peakEtCutoff = \"" << o.peakEtCutoff << '"'
       << ", jetPtCutoff = \"" << o.jetPtCutoff << '"'
       << ", chargeScaleFactor = \"" << o.chargeScaleFactor << '"'
       << ", minRecHitTime = \"" << o.minRecHitTime << '"'
       << ", maxRecHitTime = \"" << o.maxRecHitTime << '"'
       << ", randomSeed = " << o.randomSeed
       << ", centralTS = " << o.centralTS
       << ", minResponseTS = " << o.minResponseTS
       << ", maxResponseTS = " << o.maxResponseTS
       << ", minPreTS = " << o.minPreTS
       << ", maxPreTS = " << o.maxPreTS
       << ", minPostTS = " << o.minPostTS
       << ", maxPostTS = " << o.maxPostTS
       << ", mixExtraChannels = " << o.mixExtraChannels
       << ", disableChargeMixing = " << o.disableChargeMixing
        ;
    return os;
}

#endif // MixedChargeAnalysisOptions_h_
