#ifndef MixedChargeInfo_h_
#define MixedChargeInfo_h_

//
// This class contains all information necessary to mix extra charge
// to a given event represented by NoiseTreeData or another similar
// class. It works in tandem with ChargeMixingManager.
//
// The expected usage pattern of this class is as follows:
//
// 1) Create an instance of this class at the beginning of a job.
//
// 2) For every event, call the "prepareMix" method of the
//    ChargeMixingManager class. Then call the "mixWithData" method
//    of this class to add extra charge to NoiseTreeData.
//
// In addistion to the added charge, this class contains the summary
// of the corresponding added energy, number of primary vertices, etc.
// which can be examined by the analysis code.
//
// I. Volobouev
// May 2013
//

#include <memory>

#include "EventChargeInfo.h"

class MixedChargeInfo
{
public:
    enum {
        nTimeSlices = ChannelChargeInfo::nTimeSlices
    };

    // The constructor arguments are as follows:
    //
    // mixExtraChannels        -- If this argument is "true" then the channels
    //                            not present in the current event will be
    //                            mixed anyway by the "mixWithData" method.
    //
    // centralTimeSlice        -- Which time slice will be used for the
    //                            "original" event.
    //
    MixedChargeInfo(bool mixExtraChannels = false,
                    int centralTimeSlice = 4);

    // Function which adds one event with the given time slice shift.
    // Will be used by "ChargeMixingManager" to add charge incrementally.
    void addEvent(std::shared_ptr<const EventChargeInfo> eventInfo, int tsShift,
                  double scaleFactor=1.0);

    // Function which clears all collected information.
    // Does not clear configuration options (i.e., constructor parameters).
    void clear();

    // Simple inspectors
    inline bool isMixingExtraChannels() const {return mixExtraChannels_;}
    inline int centralTimeSlice() const {return centralTimeSlice_;}

    // The following function will admix the contents of this object
    // to the data object. The function returns the total number of channels
    // that have charge in them after mixing. This number will typically
    // be larger than "PulseCount" if "mixExtraChannels" constructor parameter
    // was set "true".
    template <class TreeData>
    int mixWithData(const HBHEChannelMap& chmap, TreeData* data) const;

    // Collection of event info objects
    std::vector<std::shared_ptr<const EventChargeInfo> > eventInfos;

    // Collection of corresponding time slice shifts
    std::vector<int> timeSliceShifts;

    // Charge added. Channel indexing (first array index) is according to
    // the HBHEChannelMap class.
    double addedCharge[HBHEChannelMap::ChannelCount][nTimeSlices];

    // Added energy. Summed separately for each time slice.
    double addedEnergy[HBHEChannelMap::ChannelCount][nTimeSlices];

    // Number of channel readouts mixed per time slice
    unsigned addedReadoutsTS[HBHEChannelMap::ChannelCount][nTimeSlices];

    // Number of channel readouts mixed in all time slices
    unsigned addedReadouts[HBHEChannelMap::ChannelCount];

    // Added number of events (per time slice)
    unsigned addedEvents[nTimeSlices];

    // Added number of good primary vertices. Summed separately
    // for each time slice.
    int addedNPV[nTimeSlices];

private:
    MixedChargeInfo();

    int centralTimeSlice_;
    bool mixExtraChannels_;
};

#include "MixedChargeInfo.icc"

#endif // MixedChargeInfo_h_
