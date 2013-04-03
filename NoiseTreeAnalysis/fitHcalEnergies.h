#ifndef fitHcalEnergies_h_
#define fitHcalEnergies_h_

// This function can be used to reconstruct the HCAL channel
// calibration coefficients, assuming that the reconstructed
// energy looks somethimg like E = a*T4 + b*T5 + c. Returns
// the rms residual.
//
// I. Volobouev
// April 2013

#include <vector>
#include <utility>

double fitHcalEnergies(const std::vector<std::pair<float, float> >& timeSlices45,
                       const std::vector<float>& energies,
                       double fittedCoeffs[3]);

#endif // fitHcalEnergies_h_
