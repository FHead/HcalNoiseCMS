//
// Executable for cycling over entries in root trees
//
// I. Volobouev
// March 2013
//

#include <climits>
#include <iostream>

// The next "#include" statement gets replaced
#include "ANALYSIS_HEADER_FILE"

#include "convertCSVIntoSet.h"
#include "TROOT.h"

using namespace std;

static void print_usage(const char* progname,
                        const AnalysisClass::options_type& o)
{
    cout << "\nUsage: " << progname << ' ';
    o.usage(cout);
    cout << " [-h histoRequest] [-n maxEvents] [-s] [-t treeName] [-v] "
         << "outfile infile0 infile1 ...\n" << endl;
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
    std::string treeName("ExportTree/HcalNoiseTree");
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
    catch (CmdLineError& e) {
        cerr << "Error in " << cmdline.progname() << ": "
             << e.str() << endl;
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
