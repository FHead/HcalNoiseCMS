#ifndef ChannelGroupInfo_h_
#define ChannelGroupInfo_h_

//
// Summarize combined properties of a group of channels (e.g., in an HPD)
//
// I. Volobouev
// March 2013
//

#include <cassert>
#include <cstring>
#include <algorithm>

#include "Filter10.h"

struct ChannelGroupInfo
{
    inline ChannelGroupInfo() {reset();}

    double occupancy() const
    {
        if (nMembers)
            return static_cast<double>(nReadout)/nMembers;
        else
            return 0.0;
    }

    double integratedChargeFraction() const
    {
        if (chargeSum > 0.0)
            return chargeInWindow/chargeSum;
        else
            return -1.0;
    }

    double filteredChargeFraction() const
    {
        if (chargeSum > 0.0)
            return filteredCharge/chargeSum;
        else
            return -1.0;
    }

    void reset()
    {
        nMembers = 0;
        nReadout = 0;

        memset(charge, 0, sizeof(charge));
        energySum = 0.0;
        chargeSum = 0.0;
        chargeInWindow = 0.0;
        weightedStartTSlice = -1.0;    
        startTSlice = -1.0;
        filteredCharge = 0.0;
    }

    template<class NoiseTreeData>
    void fill(const std::vector<unsigned>& members,
              const NoiseTreeData& treeData,
              const Filter10& startTimeFilter,
              const unsigned tStart, const unsigned tEnd,
              const int* pulseNumberMap,
              const unsigned* startingSlice,
              const double* filterSums)
    {
        reset();
        nMembers = members.size();
        if (!nMembers)
            return;

        assert(tStart <= tEnd);
        assert(tEnd <= 10);
        assert(pulseNumberMap);
        assert(startingSlice);
        assert(filterSums);

        double wSum = 0.0;
        double tSum = 0.0;
        for (unsigned idx=0; idx<nMembers; ++idx)
        {
            const int iPulse = pulseNumberMap[members[idx]];
            if (iPulse >= 0)
            {
                ++nReadout;
                energySum += treeData.Energy[iPulse];
                const double* ch = &treeData.Charge[iPulse][0];
                for (unsigned k=0; k<10; ++k)
                    charge[k] += ch[k];

                const double w = filterSums[iPulse];
                if (w > 0.0)
                {
                    wSum += w;
                    tSum += startingSlice[iPulse]*w;
                }
            }
        }

        if (nReadout)
        {
            chargeSum = std::accumulate(
                charge, charge + sizeof(charge)/sizeof(charge[0]), 0.0);
            chargeInWindow = std::accumulate(charge+tStart, charge+tEnd, 0.0);
            if (wSum > 0.0)
                weightedStartTSlice = tSum/wSum;

            double filtered[10];
            startTimeFilter.apply(charge, filtered);
            const unsigned t0 = std::max_element(filtered,filtered+10)-filtered;
            startTSlice = t0;
            const unsigned maxSlice = std::min(10U, tEnd - tStart + t0);
            filteredCharge = std::accumulate(charge+t0, charge+maxSlice, 0.0);
        }
    }

    unsigned nMembers;
    unsigned nReadout;

    double charge[10];
    double energySum;
    double chargeSum;
    double chargeInWindow;
    double weightedStartTSlice;
    double startTSlice;
    double filteredCharge;
};

#endif // ChannelGroupInfo_h_
