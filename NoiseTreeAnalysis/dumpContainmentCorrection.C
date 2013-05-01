// Various standard headers
#include <iostream>
#include <sstream>
#include <cassert>
#include <stdexcept>

// Command line parser
#include "CmdLine.hh"

// ROOT headers
#include "TROOT.h"
#include "TFile.h"
#include "TH1D.h"

// Local headers
#include "HcalPulseShapes.h"
#include "HcalPulseContainmentCorrection.h"


using namespace std;


static void print_usage(const char* progname)
{
    cout << "\nUsage: " << progname << " [-n nbins] [-m maxcharge] [-e maxerror]"
         << "\n       shapeNumber phase outfile\n" << endl;
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

    unsigned nbins = 1000;
    int shape;
    double phase, maxcharge = 5000, maxerror = 0.002;
    string outfile;

    try {
        cmdline.option("-n", "--nbins") >> nbins;
        cmdline.option("-m", "--maxcharge") >> maxcharge;
        cmdline.option("-e", "--maxerror") >> maxerror;

        cmdline.optend();
        if (cmdline.argc() != 3)
            throw CmdLineError("wrong number of command line arguments");
        cmdline >> shape >> phase >> outfile;

        if (nbins < 1)
            throw CmdLineError("Invalid nbins, should be positive");

        if (maxcharge <= 0.0)
            throw CmdLineError("Invalid maxcharge, should be positive");

        if (maxerror <= 0.0)
            throw CmdLineError("Invalid maxerror, should be positive");
    }
    catch (CmdLineError& e) {
        cerr << "Error in " << cmdline.progname() << ": "
             << e.str() << endl;
        print_usage(cmdline.progname());
        return 1;
    }

    // Construct shapes
    HcalPulseShapes allPulseShapes;

    // Extract the shape we need
    const HcalPulseShape* pulseShape = 0;
    try {
        pulseShape = &allPulseShapes.getShape(shape);
    } catch (std::exception& e) {
        cerr << e.what() << endl;
        return 1;
    }

    // Initialize root
    TROOT root(cmdline.progname(), "HcalPulseContainmentCorrection");
    root.SetBatch(kTRUE);

    // Open output file
    TFile rootfile(outfile.c_str(), "RECREATE");
    if (!rootfile.IsOpen())
    {
        cerr << "Failed to open file \"" << outfile << '"' << endl;
        return 1;
    }
    rootfile.cd();

    // Cycle over number of samples
    for (unsigned ns=1U; ns<6U; ++ns)
    {
        ostringstream name;
        name << "Shape " << shape << ", Phase " << phase << ", NTS " << ns;
        const std::string& names(name.str());
        TH1D* h = new TH1D(names.c_str(), names.c_str(), nbins, 0.0, maxcharge);
        h->GetXaxis()->SetTitle("Charge (fC)");
        h->GetYaxis()->SetTitle("Correction");

        HcalPulseContainmentCorrection corr(pulseShape, ns, phase, maxerror);
        const double bw = maxcharge/nbins;
        for (unsigned ibin=1; ibin<=nbins; ++ibin)
        {
            const double x = (ibin - 0.5)*bw;
            const double factor = corr.getCorrection(x);
            h->SetBinContent(ibin, factor);
        }
    }

    rootfile.Write();
    return 0;
}
