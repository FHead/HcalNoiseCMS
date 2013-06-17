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
    // for selected channels and to 0 for channels which should be dropped
    //
    virtual void select(const AnalysisClass& event,
                        std::vector<unsigned char>* mask) const = 0;
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
                               std::vector<unsigned char>* mask) const
    {
        assert(mask);
        mask->resize(event.PulseCount);
        assert(sizeof(unsigned char) == 1U);
        memset(&mask[0], 1, event.PulseCount);
    }
};

#endif // AbsChannelSelector_h_
