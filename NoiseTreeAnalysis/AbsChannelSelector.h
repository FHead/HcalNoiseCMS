#ifndef AbsChannelSelector_h_
#define AbsChannelSelector_h_

#include <vector>
#include <cassert>
#include <cstring>

//
// Interface class for selecting "good" channels
//
template <class AnalysisClass>
struct AbsChannelSelector
{
    inline virtual ~AbsChannelSelector() {}

    //
    // As the result of "select" call, the mask should be set to 1
    // for selected channels and to 0 for channels which should be dropped.
    //
    // The "parentPt" should provide the Pt of the object with which this
    // channel is associated (this object could be jet, track, etc).
    // Note that "parentPt" pointer is allowed to be NULL.
    //
    virtual void select(const AnalysisClass& event,
                        std::vector<unsigned char>* mask,
                        std::vector<double>* parentPt) = 0;
};

//
// A trivial implementation of AbsChannelSelector which selects
// all channels
//
template <class AnalysisClass>
struct AllChannelSelector : public AbsChannelSelector<AnalysisClass>
{
    inline AllChannelSelector() {}

    inline virtual ~AllChannelSelector() {}

    inline virtual void select(const AnalysisClass& event,
                               std::vector<unsigned char>* mask,
                               std::vector<double>* pt)
    {
        assert(mask);
        mask->resize(event.PulseCount);
        assert(sizeof(unsigned char) == 1U);
        memset(&(*mask)[0], 1, event.PulseCount);
        if (pt)
        {
            pt->resize(event.PulseCount);
            memset(&(*pt)[0], 0, event.PulseCount*sizeof(double));
        }
    }
};

#endif // AbsChannelSelector_h_
