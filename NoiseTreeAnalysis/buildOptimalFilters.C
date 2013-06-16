// Various standard headers
#include <fstream>
#include <iostream>
#include <cassert>
#include <cstring>
#include <utility>

// Command line parser
#include "CmdLine.hh"
#include "inputValidation.hh"

// ROOT headers
#include "TROOT.h"
#include "TFile.h"
#include "TNtuple.h"

// Local headers
#include "time_stamp.h"
#include "fitHcalCharge.h"
#include "ChannelChargeMix.h"
#include "HBHEChannelMap.h"
#include "HcalChargeFilter.h"
#include "DefaultQUncertaintyCalculator.h"

#include "geners/BinaryFileArchive.hh"
#include "geners/GenericIO.hh"

#include "npstat/stat/ArchivedNtuple.hh"
#include "npstat/stat/NtupleReference.hh"


using namespace std;
using namespace gs;
using namespace npstat;


namespace {
    struct ChannelChargeToFit
    {
        std::vector<std::array<float,10> > predictor;
        std::vector<float> response;
        std::vector<float> uncertainty;

        inline unsigned long size() const
        {
            const unsigned long sz = predictor.size();
            assert(sz == response.size());
            assert(sz == uncertainty.size());
            return sz;
        }
    };
}


static void print_usage(const char* progname)
{
    cout << "\nUsage: " << progname
         << " [-v] [-n minPoints]"
         << " [-m maxPoints]"
         << " [-s channelsToFitSimultaneously]"
         << " -a chargeUncertainty_a"
         << " -b chargeUncertainty_b"
         << " -c chargeUncertainty_c"
         << " filterOrder minPredictorTS maxPredictorTS"
         << " inputArchive fileToStoreFilters rootfile\n"
         << endl;
}


int main(int argc, char *argv[])
{
    typedef ArchivedNtuple<ChannelChargeMix> MyNtuple;

    // Parse input arguments
    CmdLine cmdline(argc, argv);
    if (argc == 1)
    {
        print_usage(cmdline.progname());
        return 0;
    }

    unsigned minPoints = 0, maxPoints = 1000000;
    unsigned channelsToFitSimultaneously = 10;
    unsigned filterOrder, minPredictorTS, maxPredictorTS;
    double uncert_a, uncert_b, uncert_c;
    string infile, outfile, rootfile;
    bool verbose;

    try {
        cmdline.option("-n", "--minPoints") >> minPoints;
        cmdline.option("-m", "--maxPoints") >> maxPoints;
        cmdline.option("-s", "--channelsToFitSimultaneously") >> channelsToFitSimultaneously;
        cmdline.require("-a") >> uncert_a;
        cmdline.require("-b") >> uncert_b;
        cmdline.require("-c") >> uncert_c;
        verbose = cmdline.has("-v", "--verbose");

        cmdline.optend();
        if (cmdline.argc() != 6)
            throw CmdLineError("wrong number of command line arguments");
        cmdline >> filterOrder >> minPredictorTS >> maxPredictorTS
                >> infile >> outfile >> rootfile;

        // Make sure that the input values provided
        // on the command line make sense
        validateRangeLELE(filterOrder, "filterOrder", 1U, 2U);
        validateRangeLELE(minPredictorTS, "minPredictorTS", 0U, 9U);
        validateRangeLTLE(maxPredictorTS, "maxPredictorTS", minPredictorTS, 10U);
        validateRangeLELE(channelsToFitSimultaneously, "channelsToFitSimultaneously",
                          1U, static_cast<unsigned>(HBHEChannelMap::ChannelCount));

        const unsigned minSz = minSampleSizeForHcalChargeFit(
            minPredictorTS, maxPredictorTS, filterOrder);
        if (!minPoints)
            minPoints = minSz;
        validateRangeLELE(minPoints, "minPoints", minSz, maxPoints);
    }
    catch (const CmdLineError& e) {
        cerr << "Error in " << cmdline.progname() << ": "
             << e.str() << endl;
        print_usage(cmdline.progname());
        return 1;
    }
    catch (const std::invalid_argument& ia) {
        cerr << "Error in " << cmdline.progname() << ": "
             << ia.what() << endl;
        print_usage(cmdline.progname());
        return 1;
    }

    // Naturally, DefaultQUncertaintyCalculator instantiated below
    // can be replaced by some other calculator derived from
    // AbsQUncertaintyCalculator
    const AbsQUncertaintyCalculator* calc = 0;
    calc = new DefaultQUncertaintyCalculator(uncert_a, uncert_b, uncert_c);

    // Get the ntuple
    BinaryFileArchive ar(infile.c_str(), "r");
    if (!ar.isOpen())
    {
        cerr << "Failed to open archive \""
             << infile << "\". Exiting." << endl;
        return 2;
    }

    const char* itemName = "Mixed Charge Ntuple";
    NtupleReference<MyNtuple> ref(ar, itemName, "");
    if (!ref.unique())
    {
        cerr << "Failed to load an Ntuple with name \""
             << itemName << "\" from archive \"" << infile
             << "\". Exiting." << endl;
        return 3;
    }
    CPP11_auto_ptr<MyNtuple> nt = ref.get(0);
    const unsigned long nRows = nt->nRows();

    // Object for storing the fit results
    std::vector<HcalChargeFilter> filters;
    filters.reserve(HBHEChannelMap::ChannelCount);

    // First element of the pair is the fit rms,
    // second is the number of events used
    std::vector<std::pair<double,unsigned> > filterAuxInfo;
    filterAuxInfo.reserve(HBHEChannelMap::ChannelCount);

    // Run fitting cycles
    unsigned lastChan = 0;
    for (unsigned icycle=0; lastChan < HBHEChannelMap::ChannelCount; ++icycle)
    {
        const unsigned firstChan = icycle*channelsToFitSimultaneously;
        lastChan = firstChan + channelsToFitSimultaneously;
        if (lastChan > HBHEChannelMap::ChannelCount)
            lastChan = HBHEChannelMap::ChannelCount;
        const unsigned nChanFitted = lastChan - firstChan;
        std::vector<ChannelChargeToFit> chanData(nChanFitted);
        ChannelChargeMix mix;

        if (verbose)
        {
            cout << time_stamp() << " : fitting channels "
                 << firstChan << " through " << lastChan - 1 << '\n';
            cout.flush();
        }

        for (unsigned long row=0; row<nRows; ++row)
        {
            nt->rowContents(row, &mix, 1UL);
            if (firstChan <= mix.channelIndex && mix.channelIndex < lastChan)
            {
                const unsigned idx = mix.channelIndex - firstChan;
                if (chanData[idx].response.size() < maxPoints)
                {
                    std::array<float,10> arr;
                    memcpy(&arr[0], &mix.Charge[0], 10*sizeof(float));
                    chanData[idx].predictor.push_back(arr);
                    chanData[idx].response.push_back(mix.chargeResponse);
                    chanData[idx].uncertainty.push_back((*calc)(mix));
                }
            }
        }

        for (unsigned idx=0; idx<nChanFitted; ++idx)
        {
            HcalChargeFilter filter;
            const unsigned nEvents = chanData[idx].size();
            if (nEvents > minPoints)
            {
                double rms = 0.0;

                switch (filterOrder)
                {
                case 1U:
                {
                    double fittedCoeffs[10], fittedIntercept;

                    rms = fitHcalCharge(chanData[idx].predictor,
                                        minPredictorTS, maxPredictorTS,
                                        chanData[idx].response,
                                        chanData[idx].uncertainty,
                                        fittedCoeffs, &fittedIntercept);
                    filter = HcalChargeFilter(fittedCoeffs, fittedIntercept,
                                              minPredictorTS, maxPredictorTS);
                }
                break;

                case 2U:
                {
                    Matrix<double> fMat;
                    double fittedCoeffs[10], fittedIntercept;

                    rms = fitHcalChargeSq(chanData[idx].predictor,
                                          minPredictorTS, maxPredictorTS,
                                          chanData[idx].response,
                                          chanData[idx].uncertainty,
                                          &fMat, fittedCoeffs, &fittedIntercept);
                    filter = HcalChargeFilter(fMat, fittedCoeffs, fittedIntercept,
                                              minPredictorTS, maxPredictorTS);
                }
                break;

                default:
                    assert(!"Incomplete switch statement");
                    break;
                }

                filterAuxInfo.push_back(std::make_pair(rms, nEvents));
            }
            else
                filterAuxInfo.push_back(std::make_pair(-1.0, 0U));
            filters.push_back(filter);
        }
    }
    delete calc; calc = 0;

    // Write out fit results
    assert(filters.size() == HBHEChannelMap::ChannelCount);
    ofstream of(outfile.c_str(), std::ios::binary);
    if (!of.is_open())
    {
        cerr << "Failed to open output binary file \""
             << outfile << "\". Exiting." << endl;
        return 4;
    }
    write_item(of, filters);
    of.close();

    // Check that the fit results can be read back
    ifstream is(outfile.c_str(), std::ios::binary);
    std::vector<HcalChargeFilter> readback;
    restore_item(is, &readback);
    is.close();
    if (readback != filters)
    {
        cerr << "Failed to read filters back from file \""
             << outfile << "\". Exiting." << endl;
        return 5;
    }

    // Write out the auxiliary info
    assert(filterAuxInfo.size() == HBHEChannelMap::ChannelCount);
    TROOT root(cmdline.progname(), "Fit");
    root.SetBatch(kTRUE);
    TFile rfile(rootfile.c_str(), "RECREATE");
    if (!rfile.IsOpen())
    {
        cerr << "Failed to open output root file \""
             << rootfile << "\". Exiting." << endl;
        return 6;
    }
    rfile.cd();
    TNtuple* rnt = new TNtuple("filterAuxInfo", "Filter Fit Info",
                               "channel:nEvents:rms");
    for (unsigned i=0; i<HBHEChannelMap::ChannelCount; ++i)
        rnt->Fill(i, filterAuxInfo[i].second, filterAuxInfo[i].first);
    rfile.Write();

    return 0;
}
