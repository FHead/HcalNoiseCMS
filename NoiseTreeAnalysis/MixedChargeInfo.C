#include <cassert>
#include "MixedChargeInfo.h"

MixedChargeInfo::MixedChargeInfo(const bool mixExtraChannels,
                                 const int centralTS)
    : centralTimeSlice_(centralTS),
      mixExtraChannels_(mixExtraChannels)
{
    clear();
}

void MixedChargeInfo::addEvent(std::shared_ptr<const EventChargeInfo> eventInfo,
                               const int tsShift, const double scaleFactor)
{
    eventInfos.push_back(eventInfo);
    timeSliceShifts.push_back(tsShift);

    // The time slice associated with this event
    const int ts = centralTimeSlice_ + tsShift;

    // Check that the TS shift is not too big (so that we can still keep
    // the info from the shifted event)
    const int nTS = static_cast<int>(nTimeSlices);
    assert(ts >= 0);
    assert(ts < nTS);

    // Figure out the index lookup table for charge shifting
    int fromIndex[nTimeSlices];
    for (int i=0; i<nTimeSlices; ++i)
    {
        fromIndex[i] = i - tsShift;
        if (fromIndex[i] < 0)
            fromIndex[i] = 0;
        if (fromIndex[i] >= nTS)
            fromIndex[i] = nTS - 1;
    }

    // Collect simple event-wide statistics
    const EventChargeInfo& info(*eventInfo);
    ++addedEvents[ts];
    addedNPV[ts] += info.NumberOfGoodPrimaryVertices;

    // Cycle over channels
    const unsigned nChannels = info.channelInfos.size();
    for (unsigned chan=0; chan<nChannels; ++chan)
    {
        const ChannelChargeInfo& ch(info.channelInfos[chan]);
        const unsigned idx = ch.channelIndex;

        ++addedReadouts[idx];
        ++addedReadoutsTS[idx][ts];
        addedEnergy[idx][ts] += scaleFactor*ch.Energy;

        // Add the charge
        for (unsigned i=0; i<nTimeSlices; ++i)
            addedCharge[idx][i] += scaleFactor*ch.Charge[fromIndex[i]];
    }
}

void MixedChargeInfo::clear()
{
    eventInfos.clear();
    timeSliceShifts.clear();

    memset(addedCharge, 0, sizeof(addedCharge));
    memset(addedEnergy, 0, sizeof(addedEnergy));
    memset(addedReadoutsTS, 0, sizeof(addedReadoutsTS));
    memset(addedReadouts, 0, sizeof(addedReadouts));
    memset(addedEvents, 0, sizeof(addedEvents));
    memset(addedNPV, 0, sizeof(addedNPV));
}
