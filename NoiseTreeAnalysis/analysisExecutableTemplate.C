//
// Executable for cycling over entries in root trees
//
// I. Volobouev
// March 2013
//

#include <climits>
#include <iostream>
#include <stdexcept>

// The next "#include" statement gets replaced
#include "ANALYSIS_HEADER_FILE"

#include "convertCSVIntoSet.h"
#include "TROOT.h"

using namespace std;

static const char* defaultTreeName = "ExportTree/HcalNoiseTree";

static void print_usage(const char* progname,
                        const AnalysisClass::options_type& o)
{
    cout << "\nUsage: " << progname << ' ';
    o.listOptions(cout);
    cout << " [-h histoRequest] [-n maxEvents] [-s] [-t treeName] [-v] "
         << "outfile infile0 infile1 ...\n" << endl;
    cout << "The required command line arguments are:\n\n";
    cout << " outfile                The name for the output root file.\n\n";
    cout << " infile0 infile1 ...    One or more names for the input root files.\n\n";
    cout << "Available command line options are:\n" << endl;
    o.usage(cout);
    cout << " -h    Comma-separated request which lists histograms and ntuples to fill.\n";
    cout << "       This request will be passed on to HistogramManager. Use '.*'\n";
    cout << "       (including single quotes) as the value of this option to fill all\n";
    cout << "       possible histograms and ntuples.\n\n";
    cout << " -n    Specify the maximum number of events to process (after cuts). If\n";
    cout << "       this option is not specified, all input events will be processed.\n\n";
    cout << " -s    Suppress summary printout at the end of program execution.\n\n";
    cout << " -t    The name of the TTree (or TChain) to process with this program.\n";
    cout << "       Default value of this option is \"" << defaultTreeName << "\".\n\n";
    cout << " -v    Verbose switch: print some diagnostics to the standard output\n";
    cout << "       as the program runs.\n" << endl;
}

int main(int argc, char *argv[])
{
    // Parse input arguments
    CmdLine cmdline(argc, argv);
    AnalysisClass::options_type opts;
    if (argc == 1)
    {
        print_usage(cmdline.progname(), opts);
        return 0;
    }

    unsigned long maxEvents = ULONG_MAX/2 - 1;
    std::string treeName(defaultTreeName);
    std::string histoRequest, outfile;
    std::vector<std::string> infiles;
    bool verbose = false;
    bool printStats = true;

    try {
        cmdline.option("-h", "--histogram") >> histoRequest;
        cmdline.option("-n", "--maxEvents") >> maxEvents;
        cmdline.option("-t", "--treeName") >> treeName;
        verbose = cmdline.has("-v", "--verbose");
        printStats = !cmdline.has("-s", "--noStats");

        opts.parse(cmdline);

        cmdline.optend();
        if (cmdline.argc() < 2)
            throw CmdLineError("wrong number of command line arguments");

        cmdline >> outfile;
        infiles.reserve(cmdline.argc());
        while (cmdline)
        {
            std::string s;
            cmdline >> s;
            infiles.push_back(s);
        }
    }
    catch (const CmdLineError& e) {
        cerr << "Error in " << cmdline.progname() << ": "
             << e.str() << endl;
        print_usage(cmdline.progname(), opts);
        return 1;
    }
    catch (const std::invalid_argument& ia) {
        cerr << "Error in " << cmdline.progname() << ": "
             << ia.what() << endl;
        print_usage(cmdline.progname(), opts);
        return 1;
    }

    // Initialize ROOT
    TROOT root("analysis", "Noise Tree");
    root.SetBatch(kTRUE);

    // Fill out the input chain
    TChain chain(treeName.c_str());
    const unsigned nFiles = infiles.size();
    for (unsigned i=0; i<nFiles; ++i)
        chain.Add(infiles[i].c_str());
    if (printStats)
    {
        cout << chain.GetEntries() << " events in the input chain\n";
        cout.flush();
    }

    // Create and run the analysis
    AnalysisClass analysis(&chain, outfile, convertCSVIntoSet(histoRequest),
                           maxEvents, verbose, opts);
    const int status = analysis.process();

    if (printStats)
    {
        // Print out basic info about the number of events processed
        cout << analysis.getProcessCounter() << " events processed" << endl;
        const Long64_t nC = analysis.getEventCounter() -
                            analysis.getProcessCounter();
        cout << nC << " additional events did not pass the cut" << endl;
    }

    return status;
}
