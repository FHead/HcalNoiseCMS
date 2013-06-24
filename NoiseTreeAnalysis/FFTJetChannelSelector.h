#ifndef FFTJetChannelSelector_h_
#define FFTJetChannelSelector_h_

#include "AbsChannelSelector.h"
#include "HBHEChannelGeometry.h"
#include "fftjetTypedefs.h"

#include "fftjet/Grid2d.hh"
#include "fftjet/Kernels.hh"
#include "fftjet/ConstScaleReconstruction.hh"
#include "fftjet/KernelRecombinationAlg.hh"
#include "fftjet/DiscreteGauss2d.hh"
#include "fftjet/FrequencyKernelConvolver.hh"
#include "fftjet/PeakSelectors.hh"
#include "fftjet/GaussianNoiseMembershipFcn.hh"

template <class AnalysisClass>
class FFTJetChannelSelector : public AbsChannelSelector<AnalysisClass>
{
public:
    typedef fftjet::RecombinedJet<VectorLike> Jet;

    FFTJetChannelSelector(const HBHEChannelGeometry& geometry,
                          unsigned nEtaBins, double etaMin, double etaMax,
                          unsigned nPhiBins, double pattRecoScale,
                          double etaToPhiBandwidthRatio, double coneSize,
                          double peakEtCutoff, double jetPtCutoff);

    inline virtual ~FFTJetChannelSelector() {}

    virtual void select(const AnalysisClass& event,
                        std::vector<unsigned char>* mask,
                        std::vector<double>* associatedJetPt);

    inline const std::vector<Jet>& getJets() const {return recoJets_;}
    inline const VectorLike& unclusteredP4() const {return unclustered_;}
    inline double sumEt() const {return sumEt_;}
    inline double unusedEt() const {return unclusScalar_;}
    inline unsigned nGoodJets() const {return jetEta_.size();}

private:
    FFTJetChannelSelector();

    // Calorimeter geometry
    const HBHEChannelGeometry& geometry_;

    // Parameters specified in the constructor
    double patRecoScale_;
    double jetPtCutoff_;

    // Cone sizes in eta and phi
    double etaConeSize_;
    double phiConeSize_;

    // Energy flow discretization grid
    fftjet::Grid2d<Real> calo_;

    // The DFFT engine
    MyFFTEngine engine_;

    // Pattern recognition convolution kernel
    fftjet::DiscreteGauss2d kernel_;

    // Convolver for the kernel
    fftjet::FrequencyKernelConvolver<Real,Complex> convolver_;

    // Peak finder
    fftjet::PeakFinder peakFinder_;

    // Peak selector (works after peak finder)
    fftjet::SimplePeakSelector peakSelector_;

    // Members needed to define the energy recombination algorithm
    fftjet::Linear2d jetMemberFcn_;
    fftjet::GaussianNoiseMembershipFcn noiseMemberFcn_;
    fftjet::KernelRecombinationAlg<Real,VectorLike,BgData,VBuilder> recoAlg_;

    // FFTJet algorithm sequence
    fftjet::ConstScaleReconstruction<Real,VectorLike,BgData> sequencer_;

    // The vector of reconstructed jets (we will refill it in every event)
    std::vector<Jet> recoJets_;

    // Jet pt, eta and phi for fast access
    std::vector<double> jetPt_;
    std::vector<double> jetEta_;
    std::vector<double> jetPhi_;

    // Unclustered 4-vector and unused transverse energy
    VectorLike unclustered_;
    double unclusScalar_;

    // Total visible transverse energy, summed as scalar
    double sumEt_;
};

#include "FFTJetChannelSelector.icc"

#endif // FFTJetChannelSelector_h_
