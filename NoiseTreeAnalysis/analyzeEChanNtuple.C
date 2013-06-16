// Various standard headers
#include <iostream>

// Command line parser
#include "CmdLine.hh"

// ROOT headers
#include "TROOT.h"
#include "TFile.h"
#include "TNtuple.h"
#include "TH1.h"

// Geners headers
#include "geners/Record.hh"

// Local headers
#include "HBHEChannelMap.h"

// NPStat headers
#include "npstat/stat/HistoAxis.hh"
#include "npstat/stat/SampleAccumulator.hh"
#include "npstat/stat/QuantileTable1D.hh"
#include "npstat/stat/LeftCensoredDistribution.hh"

#include "geners/stringArchiveIO.hh"


using namespace std;
using namespace npstat;


static void print_usage(const char* progname)
{
    cout << "\nUsage: " << progname
         << " [-n nIntervals] [-r minRatio] [-t title] [-e eventCountHisto] infile outfile\n"
         << endl;
}


int main(int argc, char *argv[])
{
    const double minusInfinity = -1024.0;

    // Parse input arguments
    CmdLine cmdline(argc, argv);
    if (argc == 1)
    {
        print_usage(cmdline.progname());
        return 0;
    }

    string evHistoName = "1-d/EventCount";
    unsigned nIntervals = 1000;
    double minFillsToIntervalsRatio = 2.0;
    string itemName = "HBHE/ChannelEnergyNtuple";
    string infile, outfile;

    try {
        cmdline.option("-n", "--nIntervals") >> nIntervals;
        cmdline.option("-r", "--minRatio") >> minFillsToIntervalsRatio;
        cmdline.option("-t", "--title") >> itemName;
        cmdline.option("-e", "--eventCountHisto") >> evHistoName;

        cmdline.optend();
        if (cmdline.argc() != 2)
            throw CmdLineError("wrong number of command line arguments");
        cmdline >> infile >> outfile;

        if (minFillsToIntervalsRatio < 1.0)
            throw CmdLineError("Invalid minRatio, should be >= 1");

        if (nIntervals == 0)
            throw CmdLineError("Invalid nIntervals, should be >= 1");
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
    TNtuple* nt = dynamic_cast<TNtuple*>(rootfile.Get(itemName.c_str()));
    if (!nt)
    {
        cerr << "Failed to load a TNtuple with name \""
             << itemName << "\" from file \"" << infile
             << "\". Exiting." << endl;
        return 1;
    }

    // Get the event counts
    TH1* h = dynamic_cast<TH1*>(rootfile.Get(evHistoName.c_str()));
    if (!h)
    {
        cerr << "Failed to load a TH1 with name \""
             << evHistoName << "\" from file \"" << infile
             << "\". Exiting." << endl;
        return 1;
    }
    const double totalEvents = h->GetBinContent(1);
    if (totalEvents <= 0.0)
    {
        cerr << "It looks like no events were processed. Exiting." << endl;
        return 1;
    }

    // Build energy samples. Run two passes over the ntuple:
    // in the first one just determine the counts, and in
    // the second allocate the memory and do the fills.
    const unsigned long n_rows = static_cast<unsigned long>(nt->GetEntries());
    unsigned long counts[HBHEChannelMap::ChannelCount] = {0,};
    for (unsigned long j=0; j<n_rows; ++j)
    {
        nt->GetEntry(j);
        const float* data = nt->GetArgs();
        const unsigned chan = data[0];
        assert(chan < HBHEChannelMap::ChannelCount);
        counts[chan]++;
    }

    // Allocate the minimal amount of memory needed for all samples
    typedef SampleAccumulator<float> Acc;
    Acc accs[HBHEChannelMap::ChannelCount];
    for (unsigned chan=0; chan<HBHEChannelMap::ChannelCount; ++chan)
        accs[chan].reserve(counts[chan]);

    // Collect the samples
    for (unsigned long j=0; j<n_rows; ++j)
    {
        nt->GetEntry(j);
        const float* data = nt->GetArgs();
        const unsigned chan = data[0];
        const float value = data[1];
        accs[chan].accumulate(value);
    }

    // No longer need the root file
    rootfile.Close();

    // Use a uninform histogram axis for quantile arguments
    const HistoAxis defaultAxis(nIntervals, 0.0, 1.0);
    std::vector<double> vbuf(nIntervals);
    double* buf = &vbuf[0];
    gs::StringArchive ar;

    for (unsigned chan=0; chan<HBHEChannelMap::ChannelCount; ++chan)
    {
        ostringstream os;
        os << chan;
        const std::string& chanString = os.str();

        if (counts[chan] < minFillsToIntervalsRatio)
        {
            // There is really nothing visible in this channel...
            buf[0] = 0.5;
            QuantileTable1D qtable(0.0, 1.0, buf, 1U);
            LeftCensoredDistribution lc(qtable, 0.0, minusInfinity);
            ar << gs::Record(lc, chanString.c_str(), "");
            continue;
        }

        // Check if we have enough events to meaningfully
        // sample quantiles using nIntervals intervals
        const HistoAxis* axis = &defaultAxis;
        unsigned long maxInt = counts[chan] / minFillsToIntervalsRatio;
        if (maxInt < nIntervals)
            axis = new HistoAxis(maxInt, 0.0, 1.0);
        else
            maxInt = nIntervals;

        // Sample empirical quantiles into a buffer
        for (unsigned long i=0; i<maxInt; ++i)
            buf[i] = accs[chan].quantile(axis->binCenter(i));

        // Scale things in such a way that minValue = 0, maxValue = 1
        const double minValue = accs[chan].min();
        const double maxValue = accs[chan].max();
        const double width = maxValue - minValue;
        assert(width > 0.0);
        for (unsigned long i=0; i<maxInt; ++i)
        {
            buf[i] -= minValue;
            buf[i] /= width;
            if (buf[i] < 0.0)
                buf[i] = 0.0;
            else if (buf[i] > 1.0)
                buf[i] = 1.0;
            if (i)
                if (buf[i] < buf[i-1])
                    buf[i] = buf[i-1];
        }

        const double occupancy = counts[chan]/totalEvents;
        QuantileTable1D qtable(minValue, width, buf, maxInt);
        LeftCensoredDistribution lc(qtable, occupancy, minusInfinity);
        ar << gs::Record(lc, chanString.c_str(), "");

        if (axis != &defaultAxis)
            delete axis;
    }

    if (!writeCompressedStringArchive(ar, outfile.c_str()))
    {
        cerr << "Failed to write archive to file \"" << outfile << '"' << endl;
        return 1;
    }

    return 0;
}
