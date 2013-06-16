#ifndef fitHcalCharge_h_
#define fitHcalCharge_h_

// Functions for reconstructing the original unmixed charge
// from charge mixtures using least squares optimization
//
// I. Volobouev
// June 2013

#include <vector>
#include <array>

#include "npstat/nm/Matrix.hh"

// The following function can be used to construct an optimal linear
// filter which gets the original charge from charge mixture.
// The rms residual of the fit is returned. The arguments are:
//
// predictor    -- Charge in time slices after mixing, one array per event
//
// minTSToFit   -- Minimum TS number to use in the fit
//
// maxTS        -- This should be larger by 1 than the maximum TS number
//                 used in the fit. Must have minTSToFit < maxTS <= 10.
//
// response     -- Actual charge before mixing
//
// uncertainty  -- Uncertainty assigned to the charge value before mixing.
//                 This can be an empty vector in which case all uncertainties
//                 are set to 1. If uncertainties are provided, they all must
//                 be positive.
//
// fittedCoeffs -- Fitted TS coefficients. The coefficients for time slices
//                 not used in the fit will be set to 0.
//
// intercept    -- Place to store the fitted intercept (i.e., the constant term)
//
double fitHcalCharge(const std::vector<std::array<float,10> >& predictor,
                     unsigned minTSToFit, unsigned maxTS,
                     const std::vector<float>& response,
                     const std::vector<float>& uncertainty,
                     double fittedCoeffs[10], double* intercept);


// The following function can be used to construct an optimal quadratic
// filter. The arguments are the same as before, with the exception of
// "fittedSqCoeffs". That matrix should be used to construct a bilinear
// form with the time slices (linear terms and intercept are added to
// this bilinear form).
//
double fitHcalChargeSq(const std::vector<std::array<float,10> >& predictor,
                       unsigned minTSToFit, unsigned maxTS,
                       const std::vector<float>& response,
                       const std::vector<float>& uncertainty,
                       npstat::Matrix<double>* fittedSqCoeffs,
                       double fittedCoeffs[10], double* intercept);

// Helper function for determining the minimum sample size.
// Parameter "fitOrder" should be set to 1 (linear) for use with
// "fitHcalCharge" and to 2 (quadratic) for use with "fitHcalChargeSq".
//
unsigned minSampleSizeForHcalChargeFit(unsigned minTSToFit, unsigned maxTS,
                                       unsigned fitOrder);

#endif // fitHcalCharge_h_
