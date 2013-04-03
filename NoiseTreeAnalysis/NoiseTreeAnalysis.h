#ifndef NoiseTreeAnalysis_h_
#define NoiseTreeAnalysis_h_

#include "RootChainProcessor.h"
#include "HistogramManager.h"
#include "HBHEChannelMap.h"
#include "HBHEChannelGeometry.h"
#include "ChannelGroupInfo.h"

#include "npstat/stat/LeftCensoredDistribution.hh"

template <class Options, class RootMadeClass>
class NoiseTreeAnalysis : public RootChainProcessor<RootMadeClass>
{
public:
    enum {
        nTimeSlices = 10
    };

    typedef Options options_type;
    typedef NoiseTreeAnalysis<Options, RootMadeClass> MyType;

    NoiseTreeAnalysis(TTree *tree, const std::string& outputfile,
                      const std::set<std::string>& histoRequest,
                      unsigned long maxEvents, bool verbose,
                      const Options& opt);

    virtual ~NoiseTreeAnalysis() {}

    inline const Options& getOptions() const {return options_;}
    inline bool isVerbose() const {return verbose_;}

    virtual Bool_t Notify();
    virtual Int_t Cut(Long64_t entryNumber);

protected:
    virtual int beginJob();
    virtual int event(Long64_t entryNumber);
    virtual int endJob();

    virtual void bookManagedHistograms();
    virtual void fillManagedHistograms();

private:
    // Options passed to us from the main program
    const Options options_;
    const bool verbose_;

    // The histogram manager
    HistogramManager manager_;

    // Channel number mapping tool
    HBHEChannelMap channelMap_;

    // HCAL geometry tool
    HBHEChannelGeometry channelGeometry_;

    // Linearized channel number (index valid up to this->PulseCount)
    unsigned channelNumber_[HBHEChannelMap::ChannelCount];

    // Lookup table from channel number into the sequence in this
    // tree. Filled with -1 if the channel was not read out.
    int pulseNumber_[HBHEChannelMap::ChannelCount];

    // HPD and RBX numbers for this channel (up to this->PulseCount)
    unsigned hpdNumber_[HBHEChannelMap::ChannelCount];
    unsigned chanInHpdNumber_[HBHEChannelMap::ChannelCount];
    unsigned rbxNumber_[HBHEChannelMap::ChannelCount];
    unsigned chanInRbxNumber_[HBHEChannelMap::ChannelCount];

    // Total charge and pedestal in 10 time slices (up to this->PulseCount)
    double chargeSums_[HBHEChannelMap::ChannelCount];
    double pedSums_[HBHEChannelMap::ChannelCount];

    // Charge and pedestal integrated over selected time slices
    // (with index valid up to this->PulseCount)
    double integSums_[HBHEChannelMap::ChannelCount];
    double integPeds_[HBHEChannelMap::ChannelCount];

    // Ratio of the charge inside selected time window
    // to the total charge (up to this->PulseCount)
    double signalFraction_[HBHEChannelMap::ChannelCount];

    // Determine the time slice where the signal starts (up to this->PulseCount)
    unsigned startingSlice_[HBHEChannelMap::ChannelCount];

    // Charge in the time slices determined by the filter
    // (up to this->PulseCount)
    double filterSums_[HBHEChannelMap::ChannelCount];

    // Summary info for channels grouped by HPDs
    ChannelGroupInfo hpdInfo_[HcalHPDRBXMap::NUM_HPDS];

    // Summary info for "static" HPD neighbor channels
    // (neighbors of all HPD channels, both read out and not)
    ChannelGroupInfo staticNeighborInfo_[HcalHPDRBXMap::NUM_HPDS];

    // Summary info for "dynamic" HPD neighbor channels
    // (neighbors of those HPD channels that have been read out)
    ChannelGroupInfo dynamicNeighborInfo_[HcalHPDRBXMap::NUM_HPDS];

    // Channel numbers read out for each HPD in this event
    std::vector<unsigned> hpdChannelsReadOut_[HcalHPDRBXMap::NUM_HPDS];

    // "Dynamic" neighbor channels for each HPD in this event
    std::vector<unsigned> hpdNeighbors_[HcalHPDRBXMap::NUM_HPDS];

    // Channel occupancy per RBX
    double rbxOccupancy_[HcalHPDRBXMap::NUM_RBXS];

    // Table of distributions which convert energy values seen
    // into occupancy above that energy and back
    typedef std::shared_ptr<npstat::LeftCensoredDistribution> OccConverterPtr;
    std::vector<OccConverterPtr> occupancyCoverters_;

    // Internal helper functions
    void loadOccupancyConverters();
    double calculatePseudoLogLikelihood(const std::vector<unsigned>& ch) const;

    double staticSignalPseudoLogli(const unsigned hpd) const
        {return calculatePseudoLogLikelihood(channelMap_.getHPDChannels(hpd));}

    double dynamicSignalPseudoLogli(const unsigned hpd) const
        {return calculatePseudoLogLikelihood(hpdChannelsReadOut_[hpd]);}

    double staticNeighborPseudoLogli(const unsigned hpd) const
        {return calculatePseudoLogLikelihood(channelMap_.getHPDNeigbors(hpd));}

    double dynamicNeighborPseudoLogli(const unsigned hpd) const
        {return calculatePseudoLogLikelihood(hpdNeighbors_[hpd]);}
};

#include "NoiseTreeAnalysis.icc"

#endif // NoiseTreeAnalysis_h_
