#ifndef CHANNELCHARGEMIX_H_
#define CHANNELCHARGEMIX_H_

//
// Persistent struct which contains charge-related information for a single
// HBHE channel after charge mixing (in preparation for fitting the filter)
//
// I. Volobouev
// June 2013
//

#include <climits>

#include "MixedChargeInfo.h"
#include "geners/ClassId.hh"

struct ChannelChargeMix
{
    enum {
        nTimeSlices = ChannelChargeInfo::nTimeSlices
    };

    // Default constructor creates a dummy object with invalid channel
    // index. Charge arrays will remain uninitialized.
    //
    inline ChannelChargeMix()
        : chargeResponse(0.0),
          Energy(0.0),
          RecHitTime(0.0),
          FlagWord(0),
          AuxWord(0),
          channelIndex(UINT_MAX)
    {
    }

    //
    // Create this object from NoiseTreeData or another similar class.
    // "treeIndex" is the "pulse number". "hbheIndex" is the id assigned
    // to this channel by the HBHEChannelMap class.
    //
    template<class TreeData>
    inline ChannelChargeMix(const TreeData& data,
                            const MixedChargeInfo& mixedCharge,
                            const double chargeToReconstruct,
                            const unsigned treeIndex,
                            const unsigned hbheIndex)
        : chargeResponse(chargeToReconstruct),
          Energy(data.Energy[treeIndex]),
          RecHitTime(data.RecHitTime[treeIndex]),
          FlagWord(data.FlagWord[treeIndex]),
          AuxWord(data.AuxWord[treeIndex]),
          channelIndex(hbheIndex)
    {
        for (unsigned i=0; i<nTimeSlices; ++i)
            Charge[i] = data.Charge[treeIndex][i];

        for (unsigned i=0; i<nTimeSlices; ++i)
            addedCharge[i] = mixedCharge.addedCharge[hbheIndex][i];
    }

    // The meaning of the members whose name starts with a capital letter
    // is the same as for the identically named members of NoiseTreeData.
    // "Charge" here is after mixing.
    float Charge[nTimeSlices];
    float addedCharge[nTimeSlices];
    float chargeResponse;
    double Energy;
    double RecHitTime;
    unsigned FlagWord;
    unsigned AuxWord;

    // channelIndex is the channel id assigned by the HBHEChannelMap class
    unsigned channelIndex;

    // Operators which compare objects for equality are useful for I/O testing
    bool operator==(const ChannelChargeMix& r) const;
    inline bool operator!=(const ChannelChargeMix& r) const
        {return !(*this == r);}

    // I/O methods needed for writing
    inline gs::ClassId classId() const {return gs::ClassId(*this);}
    bool write(std::ostream& of) const;

    // I/O methods needed for reading
    static inline const char* classname() {return "ChannelChargeMix";}
    static inline unsigned version() {return 1;}
    static void restore(const gs::ClassId& id, std::istream& in,
                        ChannelChargeMix* ptr);
};

#endif // CHANNELCHARGEMIX_H_
