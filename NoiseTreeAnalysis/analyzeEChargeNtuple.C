// Various standard headers
#include <set>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cassert>

// Command line parser
#include "CmdLine.hh"

// ROOT headers
#include "TROOT.h"
#include "TFile.h"
#include "TNtuple.h"

// Local headers
#include "fitHcalEnergies.h"
#include "HBHEChannelMap.h"


using namespace std;


static void print_usage(const char* progname)
{
    cout << "\nUsage: " << progname
         << " [-n minPoints] [-m maxPoints] infile outfile\n"
         << endl;
}


int main(int argc, char *argv[])
{
    // Parse input arguments
    CmdLine cmdline(argc, argv);
    if (argc == 1)
    {
        print_usage(cmdline.progname());
        return 0;
    }

    unsigned minPoints = 5, maxPoints = 500;
    string infile, outfile;

    try {
        cmdline.option("-n", "--minPoints") >> minPoints;
        cmdline.option("-m", "--maxPoints") >> maxPoints;

        cmdline.optend();
        if (cmdline.argc() != 2)
            throw CmdLineError("wrong number of command line arguments");
        cmdline >> infile >> outfile;

        if (minPoints < 4)
            throw CmdLineError("Invalid minPoints, should be >= 4");

        if (maxPoints < minPoints)
            throw CmdLineError("Invalid maxPoints, should be >= minPoints");
    }
    catch (CmdLineError& e) {
        cerr << "Error in " << cmdline.progname() << ": "
             << e.str() << endl;
        print_usage(cmdline.progname());
        return 1;
    }

    // Initialize root
    TROOT root(cmdline.progname(), "Analysis");
    root.SetBatch(kTRUE);

    // Get the ntuple
    TFile rootfile(infile.c_str(), "READ");
    std::string itemName("HBHE/ChannelEnergyDependenceOnCharge");
    TNtuple* nt = dynamic_cast<TNtuple*>(rootfile.Get(itemName.c_str()));
    if (!nt)
    {
        cerr << "Failed to load a TNtuple with name \""
             << itemName << "\" from file \"" << infile
             << "\". Exiting." << endl;
        return 1;
    }
    const unsigned long n_rows = static_cast<unsigned long>(nt->GetEntries());

    // File to store the results
    ofstream of(outfile.c_str());
    if (!of.is_open())
    {
        cerr << "Failed to open output file \"" << outfile
             << "\". Exiting." << endl;
        return 2;
    }
    of.precision(12);

    // Cycle over the ntuple and figure out the run numbers
    std::set<unsigned> runNumbers;
    for (unsigned long j=0; j<n_rows; ++j)
    {
        nt->GetEntry(j);
        const float* data = nt->GetArgs();
        const unsigned run = data[0];
        runNumbers.insert(run);
    }

    // Cycle over the run numbers. Each run will be processed
    // separately to make sure we don't run out of memory.
    for (std::set<unsigned>::const_iterator it = runNumbers.begin();
         it != runNumbers.end(); ++it)
    {
        const unsigned thisRun = *it;
        std::vector<std::vector<std::pair<float,float> > > channels(4U*HBHEChannelMap::ChannelCount);
        std::vector<std::vector<float> > energies(4U*HBHEChannelMap::ChannelCount);

        for (unsigned long j=0; j<n_rows; ++j)
        {
            nt->GetEntry(j);
            const float* data = nt->GetArgs();
            const unsigned run = data[0];
            if (run == thisRun)
            {
                const unsigned chan = data[1];
                assert(chan < HBHEChannelMap::ChannelCount);

                const unsigned capId = data[2];
                assert(capId < 4U);

                const unsigned idx = 4U*chan + capId;
                const unsigned nCollected = energies[idx].size();

                if (nCollected < maxPoints)
                {
                    const float e = data[3];
                    const float ts4 = data[4];
                    const float ts5 = data[5];

                    energies[idx].push_back(e);
                    channels[idx].push_back(std::pair<float,float>(ts4,ts5));
                }
            }
        }

        for (unsigned idx=0; idx<4U*HBHEChannelMap::ChannelCount; ++idx)
        {
            const unsigned nCollected = energies[idx].size();
            if (nCollected >= minPoints)
            {
                const unsigned chan = idx / 4U;
                const unsigned capId = idx % 4U;

                double coeffs[3];
                const double sqr = fitHcalEnergies(channels[idx], energies[idx], coeffs);
                const double eTotal = std::accumulate(energies[idx].begin(), energies[idx].end(), 0.0);
                of << thisRun << "  " << chan << "  " << capId << "  "
                   << coeffs[0] << "  " << coeffs[1] << "  " << coeffs[2] << "  "
                   << nCollected << "  " << sqr << "  "
                   << eTotal/nCollected << endl;
            }
        }
    }

    return 0;
}
