#ifndef LeadingJetChannelSelector_h_
#define LeadingJetChannelSelector_h_

template <class AnalysisClass>
class LeadingJetChannelSelector : public AbsChannelSelector<AnalysisClass>
{
public:
    LeadingJetChannelSelector(const HBHEChannelGeometry& geometry,
                              double coneSize, double etaToPhiBandwidthRatio,
                              double hadronicPtCutoff);

    inline virtual ~LeadingJetChannelSelector() {}

    virtual void select(const AnalysisClass& event,
                        std::vector<unsigned char>* mask,
                        std::vector<double>* parentPt);
private:
    LeadingJetChannelSelector();

    // Calorimeter geometry
    const HBHEChannelGeometry& geometry_;

    // Parameters specified in the constructor
    double hadronicPtCutoff_;

    // Cone sizes in eta and phi
    double etaConeSize_;
    double phiConeSize_;
};

#include "LeadingJetChannelSelector.icc"

#endif // LeadingJetChannelSelector_h_
