#ifndef CHANNELCHARGEINFO_H_
#define CHANNELCHARGEINFO_H_

#include <cstring>

struct ChannelChargeInfo
{
    enum {
        nTimeSlices = 10U
    };

    template<class TreeData>
    inline ChannelChargeInfo(const TreeData& data,
                             const unsigned treeIndex,
                             const unsigned hbheIndex)
        : Energy(data.Energy[treeIndex]),
          RecHitTime(data.RecHitTime[treeIndex]),
          FlagWord(data.FlagWord[treeIndex]),
          AuxWord(data.AuxWord[treeIndex]),
          channelIndex(hbheIndex)
    {
        memcpy(Charge, &data.Charge[treeIndex][0], sizeof(Charge));
    }

    // The meaning of the following members is the same
    // as for the identically named members of NoiseTreeData
    double Charge[nTimeSlices];
    double Energy;
    double RecHitTime;
    unsigned FlagWord;
    unsigned AuxWord;

    // channelIndex is the channel id assigned by the HBHEChannelMap class
    unsigned channelIndex;

private:
    ChannelChargeInfo();
};

#endif // CHANNELCHARGEINFO_H_
