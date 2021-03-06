#ifndef EventChargeInfo_h_
#define EventChargeInfo_h_

//
// Struct which contains charge-related information for a single event
//
// I. Volobouev
// May 2013
//

#include <vector>

#include "ChannelChargeInfo.h"
#include "HBHEChannelMap.h"

struct EventChargeInfo
{
    //
    // Create this object from NoiseTreeData or another similar class
    //
    template<class TreeData>
    inline EventChargeInfo(const TreeData& data,
                           const HBHEChannelMap& chmap)
        : RunNumber(data.RunNumber),
          EventNumber(data.EventNumber),
          NumberOfGoodPrimaryVertices(data.NumberOfGoodPrimaryVertices)
    {
        channelInfos.reserve(data.PulseCount);
        for (int i=0; i<data.PulseCount; ++i)
        {
            const unsigned chNum = chmap.linearIndex(
                data.Depth[i], data.IEta[i], data.IPhi[i]);
            channelInfos.push_back(ChannelChargeInfo(data, i, chNum));
        }
    }

    // Collecion of objects which carry charge information
    // for individual channels
    std::vector<ChannelChargeInfo> channelInfos;

    // Information about this event from NoiseTreeData
    long long RunNumber;
    long long EventNumber;
    int NumberOfGoodPrimaryVertices;

private:
    EventChargeInfo();
};

#endif // EventChargeInfo_h_
