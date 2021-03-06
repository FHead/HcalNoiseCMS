
Most files in this directory belong to two major groups: the tools for
the HCAL noise analysis and the code which organizes the processing of
root trees in a manner which, I believe, is much more convenient,
scalable, and maintainable than the use of CINT scripts. The
considerations underlying this "no-CINT" approach are listed below.

First, I did not want to rely on CINT because it is at odds with the
C++ standard. It has its own strange extensions, does not handle
templates well, and it might actually have hidden discrepancies with
the standard. In CINT, std::abs(0.5) used to be 0 instead of 0.5 for
a long, long time (even after #include <cmath> statement). While this
particular problem is now fixed, others might still be there,
undetected or ignored. Another good reason not to use CINT is that
eventually some of our analysis code will have to be ported to
CMSSW. It will be easier to do this if all of our analysis code is
compiled with a standard-conforming compiler, with interactive root
reserved just for plotting.

Second, I find it much easier to deal with histogram creation, filling,
etc. if all of it is done in one place. The code which creates
histograms (and/or ntuples) in one place and fills them in another
becomes difficult to read as the number of histograms grows. Such
a code is also more difficult to debug if something is not working as
expected. This problem is exacerbated if the histograms are created
and filled conditionally, depending on the values of some boolean
switches. So, one of the purposes of the root tree processing
framework implemented here is to eliminate this inconvenience. The
framework provides wrappers in which the histogram creation parameters
are specified together with the mechanisms by which the histograms are
filled, in a single function call.

Third, it becomes easier to link external libraries with root if the
code is compiled into a standalone executable. To be sufficiently
flexible, this executable should have a reasonable command line
parsing facility (which is provided).

Finally, some thought was given to the potential evolution of the analysis
code and the underlying TTree structure. The framework strives to make
such evolution easier, partitioning the code into classes in such a way
that potential modifications are expected to remain localized.

In its present form, the analysis code relies on several tools provided
by the "NPStat" package ( http://npstat.hepforge.org ). "NPStat"
complements root statistical facilities in places where they are
lacking, such as nonparametric density estimation and regression. In
particular, "NPStat" makes it trivial to convert collections of charge
or energy measurements into proper statistical distributions, taking
into account the fact that what we really observe are just tails
visible after thresholding. I use such distributions to construct
pseudo-likelihoods for observing a set of values in an HPD. This could
potentially be used to discriminate against direct HPD hits by a
particle flying through. "NPStat", in turn, depends on several other
packages. All of these packages except one are typically included in
standard Linux distros such as Fedora or Scientific Linux. The one
which is not included is called "Geners" ( http://geners.hepforge.org ).
Installing "Geners" and "NPStat" amounts to the usual sequence of
"configure", "make", and "make install" with one twist: the C++11
support has to be explicitly enabled by setting the CXXFLAGS
environmental variable prior to running "configure". An example script
which runs the proper sequence is provided for csh-like shell: see
file "install_dependencies.csh".

"NPStat" is also much more reliable than root for calculating basic
statistical quantities. To see what I mean, paste the following into
an interactive root session:

TH1D h("UniformDistribution", "Look at the RMS!", 100, 10000016, 10000017);
for (i=1; i<=100; ++i) h.SetBinContent(i, 1.0);
h.Draw()

RMS of the flat distribution on a unit interval should be 1/sqrt(12),
up to a small discretization correction. Yet, root thinks that in this
particular example it is 0. This is because root is calculating the RMS
using a single-pass method which suffers from subtractive cancellation
(if "subtractive cancellation" sounds unfamiliar, look up, for example,
http://highenergy.phys.ttu.edu/~igv/ComputationalPhysics/Lectures/lecture1.pdf).
And yes, root developers were informed about this particular flaw many
years ago -- they don't think it is a bug, they think it is a feature.
I can't help but wonder how many "features" like that are lurking in
the root code, messing up data analyses of unsuspecting users.

Local (HBHE only) jet reconstruction is performed with the help of FFTJet
package ( http://fftjet.hepforge.org ).

Here is the list of files included in this directory, with brief
descriptions. The files are loosely grouped by their purpose.

Files implementing the root tree processing framework
-----------------------------------------------------

AllPass.h             -- Default ntuple filling selector (nothing is rejected).

analysisExecutableTemplate.C  -- Template for the framework "main" function.
                         The actual executables will be generated from this
                         template by "make".

AutoH1D.h             -- Wrappers for root histogramming classes. They make
AutoH2D.h                histograms which know how to fill themselves once
AutoH3D.h                per event.

AutoNtuple.h          -- Wrappers for ntuples which know how to fill
                         themselves once per event.

CmdLine.hh            -- Command line parsing facility. Kind of like
                         "getopt_long" but implemented in C++ and with
                         a more convenient interface.

Column.h              -- Column definition for automatically filled ntuples.

CycledH1D.h           -- Wrappers for root histogramming classes. They make
CycledH2D.h              histograms which know how to fill themselves multiple
CycledH3D.h              times per event.

CycledNtuple.h        -- Wrappers for ntuples which know how to fill
                         themselves multiple times per event.

Functors.h            -- Functor classes for use with automatically filled
                         histograms and ntuples. These classes carry the
                         information on how to fill a histogram/ntuple.

HistogramManager.h    -- Manager class for automatically filled histograms
HistogramManager.C       and ntuples.

inputValidation.hh    -- Generic validation for input parameters. The functions
                         in this file check that the parameter falls in some
                         range and throw std::invalid_argument (with a
                         meaningful description) in case it does not.

ManagedHisto.h        -- Base class for automatically filled histograms
                         and ntuples. HistogramManager holds collections
                         of classes derived from it.

NtuplePacker.h        -- Helper code for making automatically filled ntuples.

ntupleUtils.h         -- Helper code for processing simple ntuples.

RootChainProcessor.h  -- This class implements a standard cycle over TTree
                         entries. It replaces the "Loop" method in the code
                         generated by the root "MakeClass" facility. It also
                         defines the interface for user-developed analysis
                         classes which are supposed to inherit from
                         RootChainProcessor.


A simple example of how to apply the root tree processing framework
-------------------------------------------------------------------

You don't really need to know how the framework works under the hood
in order to be able to use it. Read "analysis_framework.txt", look at
the examples below (the example code has plenty of comments) and do
something similar in your programs.

ExampleAnalysis.h        -- The main example analysis class. Inherits
ExampleAnalysis.icc         from RootChainProcessor.

ExampleAnalysisOptions.h -- Example class which contains command line options.

exampleTreeAnalysis.ana  -- Analysis definition file for generating the
                            executable which uses the ExampleAnalysis class.
                            This executable will be automatically generated
                            by "make".


Tools specific to the HCAL noise analysis
-----------------------------------------

Below, when I say that the code was "lifted" from some CMSSW file, it
means that various CMSSW and boost dependencies were removed. As a minimum,
all exceptions were converted to standard library exceptions instead of
edm::Exception.

AbsChannelSelector.h -- Interface class for implementing the code which
                        selects "good" channels.

AbsQUncertaintyCalculator.h -- This header files defines an interface
                        for calculating the channel charge determination
                        uncertainty.

analyzeEChanNtuple.C -- Helper executable for building energy distributions
                        out of observed samples.

analyzeEChargeNtuple.C -- Fit 2-d linear regression model to the energy
                          dependence on charge in TS4 and TS5.

buildOptimalFilters.C -- Helper executable for building optimal charge
                         filters (see also "fitHcalCharge.h" description).

ChannelChargeInfo.h  -- A struct which contains charge-related information
                        for a single HBHE channel.

ChannelChargeMix.h   -- A persistent struct which contains charge-related
ChannelChargeMix.C      information for a single HBHE channel after charge
                        mixing has been performed.

ChargeMixingManager.h -- The code which loads events (and later combines
ChargeMixingManager.icc  time-shifted versions of them) for charge mixing.

ChannelGroupInfo.h   -- Summary info for a group of channels. Used to
                        study channels grouped by HPD as well as those
                        channels neighboring an HPD.

DefaultQUncertaintyCalculator.h -- A simple implementation of charge
DefaultQUncertaintyCalculator.C    uncertainty calculator (inherits from
                                   AbsQUncertaintyCalculator). The uncertainty
                                   depends only on charge Q itslef according to
                                   the formula a*Q + b*sqrt(Q) + c with some
                                   constants a, b, and c.

DetId.h              -- The CMS detector id. Lifted from CMSSW code
                        DataFormats/DetId/interface/DetId.h.

dumpContainmentCorrection.C -- Executable for saving pulse shape corrections
                               as histograms for subsequent visualization.

EventChargeInfo.h    -- A struct which contains charge-related information
                        for a complete event, in a form suitable for
                        subsequent mixing.

Filter10.h           -- Linear filtering for the time structure of HCAL pulses.

fitHcalCharge.h      -- Functions for building optimal linear and quadratic
fitHcalCharge.C         filters which reconstruct the original signal charge
                        from charge mixtures.

fitHcalEnergies.h    -- Code which can be used to fit 2-d linear regression
fitHcalEnergies.C       models (such as energy dependence on two time slices)

genlkupmap.h         -- Apparently, some kind of a lookup table generation
                        utility, internal to the HCAL pulse shape correction
                        code. Lifted from CMSSW file
                        CalibCalorimetry/HcalAlgos/interface/genlkupmap.h.

HBHEChannelMap.h     -- Numerology for finding collections of HCAL channels
HBHEChannelMap.C        that belong the same HPD and/or for navigating
                        collections of HPD neighbor channels (neighbors
                        are found by changing ieta or iphi by one and checking
                        that they do not belong to the same HPD).

HBHEChannelGeometry.h -- Geometrical information for HB and HE channels.
HBHEChannelGeometry.C    This is, basically, a lookup table of channel
                         physical directions by channel number.

HcalChargeFilter.h   -- Charge calculation after mixing using optimal
HcalChargeFilter.C      regression coefficients. Provides persistent
                        storage for regression coefficients for one channel.

HcalDetId.h          -- Numerology for naming HCAL channels. Used internally
HcalDetId.C             by HBHEChannelMap and HcalHPDRBXMap. Lifted from
                        DataFormats/HcalDetId/interface/HcalDetId.h.

HcalHPDRBXMap.h      -- Numerology for finding collections of HCAL channels
HcalHPDRBXMap.C         that belong the same RBX. Lifted from CMSSW code
                        RecoMET/METAlgorithms/interface/HcalHPDRBXMap.h.

HcalPulseContainmentAlgo.h -- Pulse shape correction algorithm parameters.
HcalPulseContainmentAlgo.C    Lifted from CMSSW code
                     CalibCalorimetry/HcalAlgos/src/HcalPulseContainmentAlgo.h.

HcalPulseContainmentCorrection.h -- Pulse shape correction code.
HcalPulseContainmentCorrection.C    Lifted from CMSSW code
         CalibCalorimetry/HcalAlgos/interface/HcalPulseContainmentCorrection.h.

HcalPulseShape.h     -- HCAL pulse shape management (by number). Lifted
HcalPulseShape.C        from CMSSW sources at
HcalPulseShapes.h       CalibCalorimetry/HcalAlgos/interface/HcalPulseShape.h,
HcalPulseShapes.C       CalibCalorimetry/HcalAlgos/interface/HcalPulseShapes.h.

HcalShapeIntegrator.h -- HCAL pulse shape integrator. Lifted from CMSSW
HcalShapeIntegrator.C    sources at
                    CalibCalorimetry/HcalAlgos/interface/HcalShapeIntegrator.h.

HcalSubdetector.h    -- Definition of HCAL subdetectors. Lifted from
                        DataFormats/HcalDetId/interface/HcalSubdetector.h

HcalTimeSlew.h       -- HCAL QIE pulse delay as a function of amplitude. From
HcalTimeSlew.C          CalibCalorimetry/HcalAlgos/interface/HcalTimeSlew.h.

JetSummary.h         -- Summary of local (HBHE only) jet reconstruction.

LeadingJetChannelSelector.h  -- Channel selector using the two leading jets.
LeadingJetChannelSelector.icc

NoiseTreeAnalysis.h  -- A data analysis class which can be used to
NoiseTreeAnalysis.icc   histogram almost all variables in the TTree defined
                        by Yi Chen (with exception of event id variables,
                        various flags and trigger bits). It is useful for
                        a number of other things as well. This code is under
                        development, and it will be worthy of dedicated
                        documentation at some point. For now, however, please
                        just read the code (it does help to look at the simple
                        example first).

NoiseTreeAnalysisOptions.h -- Command line options for NoiseTreeAnalysis.

NoiseTreeData.h      -- Generated by the root "MakeClass" facility from the
NoiseTreeData.C         noise study TTree.

making_a_class.C     -- Example script which runs "MakeClass" in an interactive
                        root session.

make_configFile.py   -- Script for configuring charge mixing probabilities and
                        generators.

MixedChargeAnalysis.h  -- A data analysis class to analyze events in which
MixedChargeAnalysis.icc   additional charge is mixed from other events.

MixedChargeAnalysisOptions.h -- Command line options for MixedChargeAnalysis.

MixedChargeInfo.h    -- This class contains all information necessary to
MixedChargeInfo.icc     mix extra charge to a given event represented by
MixedChargeInfo.C       NoiseTreeData or another similar class. It works
                        in tandem with ChargeMixingManager.

npstat.py            -- SWIG-generated python wrapper for some of the facilities
npstat_wrap_v2_0_0.cc   included in the "Geners" and "NPStat" packages.

runNoiseTreeAnalysis.ana -- Analysis definition file for generating the
                        executable which creates and uses the NoiseTreeAnalysis
                        class.

runMixedChargeAnalysis.ana -- Analysis definition file for generating the
                        executable which uses the MixedChargeAnalysis class.

FFTJetChannelSelector.h   -- These files perform selection of "good" channels
FFTJetChannelSelector.icc    by associating them with energetic jets locally
                             reconstructed by FFTJet.

fftjetTypedefs.h     -- Basic typedefs needed for FFTJet to work.
VBuilders.h


Miscellaneous utilities
-----------------------

convertCSVIntoSet.h  -- A helper function for command line processing.

time_stamp.h         -- Current time as an std::string.

deltaPhi.h           -- Delta phi difference overloaded for scalars and vectors

skipComments.h       -- Load lines from a file skipping pure white space
skipComments.C          lines and comment lines. Comment lines are the lines
                        starting with # after, possibly, some amount of white
                        space.

Makefiles
---------

Makefile             -- The main makefile which builds all executables.
                        Take a look at, edit if necessary, and type "make".

Makefile.tools       -- Helper makefile for building executables that
                        do not use the root tree processing framework.
                        Will be used by the main makefile.

Documentation
-------------

00README.txt             -- This file.

analysis_framework.txt   -- Brief description of the framework usage.

install_dependencies.csh -- Script which shows how to install "Geners"
                            and "NPStat".

setup.csh                -- Script to use for updating LD_LIBRARY_PATH
                            before running any executable.

Most header files are reasonably well commented; doxygen formatting
statements should probably be added there as well.


Igor Volobouev
June 18, 2013
