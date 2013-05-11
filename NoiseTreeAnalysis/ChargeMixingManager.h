#ifndef CHARGEMIXINGMANAGER_H_
#define CHARGEMIXINGMANAGER_H_

//
// This class performs most of the work related to charge mixing
// management for HCAL noise studies. This includes creation of
// the TChain used for mixing, loading distributions for the number
// of mixed events and time shifts, generation of appropriate random
// numbers, etc.
//
// The expected usage of this class is as follows:
//
// 1) Create one instance of it per job.
//
// 2) Call "loadEventsToMix" at the beginning of a job, before
//    going into the event cycle.
//
// 3) For every event, call "prepareMix" method which will
//    fill out an instance of "MixedChargeInfo" object. Then
//    call "mixWithData" method of MixedChargeInfo to add extra
//    charge to NoiseTreeData.
//
// It might make sense to derive from this class and override the
// "Cut" function if you don't like the default filter used to
// select admixed events.
//
// I. Volobouev
// May 2013
//

#include <string>

#include "npstat/rng/AbsRandomGenerator.hh"
#include "npstat/stat/AbsDiscreteDistribution1D.hh"

#include "MixedChargeInfo.h"
#include "HBHEChannelMap.h"

template <class RootMadeClass>
class ChargeMixingManager
{
public:
    //
    // "objectConfigFile" constructor argument is the name of the file
    // which contains the distributions which we will generate for the
    // number of events to add and for time slice shifts.
    //
    // If "verbose" is true, the manager will print some diagnostics
    // to the standard output while it operates.
    //
    ChargeMixingManager(const std::string& objectConfigFile, bool verbose);

    inline virtual ~ChargeMixingManager() {}

    // The following function should be called at the beginning
    // of the job to load the chain of mixed events. Returns "true"
    // on success and "false" on failure. The argument are as follows:
    //
    // fileWithFileNames -- The name of text file which contains the
    //                      file names of root files with events that
    //                      will be used as sources of admixed charge.
    //                      These file names should be included using
    //                      one entry per line. Such a file can be easily
    //                      created by "ls", for example,
    //                      ls /this/and/that/NoiseTree_*.root > list.txt
    //
    // mixedTreeName     -- The tree name in the root files, typically 
    //                      should be set to "ExportTree/HcalNoiseTree"
    //
    // chmap             -- Channel numbering scheme
    //
    // Note that this code will attempt to load the charge-related event
    // information into memory from all events that pass the selection cut.
    // It is therefore can be unwise to load too many events, as the computer
    // memory can be exhausted. The loading operation by itself is not very
    // fast either -- on my laptop the speed is about 500 events/sec.
    //
    virtual bool loadEventsToMix(const std::string& fileWithFileNames,
                                 const char* mixedTreeName,
                                 const HBHEChannelMap& chmap);

    // Check how many events were loaded by "loadEventsToMix"
    inline unsigned long mixedEventCount() const {return events.size();}

    // The following function fills MixedChargeInfo object
    // with properly randomized events (MixedChargeInfo
    // will be cleared initially). Should normally be called
    // for every event.
    virtual void prepareMix(npstat::AbsRandomGenerator& rng,
                            const HBHEChannelMap& chmap,
                            MixedChargeInfo* info) const;
private:
    typedef std::shared_ptr<const EventChargeInfo> EPtr;

    ChargeMixingManager();
    ChargeMixingManager(const ChargeMixingManager&);
    ChargeMixingManager* operator=(const ChargeMixingManager&);

    // The "Cut" function should return 1 if entry is accepted for mixing
    // and -1 otherwise (same convention as in "root"). Override as necessary.
    inline virtual int Cut(const RootMadeClass& dataTree) const
    {
        if (dataTree.NumberOfGoodPrimaryVertices > 0 &&
            dataTree.NumberOfGoodTracks > 0)
            return 1;
        else
            return -1;
    }

    std::vector<EPtr> events;
    std::unique_ptr<npstat::AbsDiscreteDistribution1D> eventDistro;
    std::unique_ptr<npstat::AbsDiscreteDistribution1D> tsShiftDistro;
    bool verbose_;
};

#include "ChargeMixingManager.icc"

#endif // CHARGEMIXINGMANAGER_H_
