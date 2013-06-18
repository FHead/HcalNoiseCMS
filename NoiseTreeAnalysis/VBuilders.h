#ifndef VBuilders_h_
#define VBuilders_h_

#include <cmath>

#include "TLorentzVector.h"

struct PtEtaP4Builder
{
    inline TLorentzVector operator()(
        const double pt, const double eta, const double phi) const
        {
            const double px = pt*cos(phi);
            const double py = pt*sin(phi);
            const double pz = pt*sinh(eta);
            const double e = sqrt(px*px + py*py + pz*pz);
            return TLorentzVector(px, py, pz, e);
        }
};

struct EnergyEtaP4Builder
{
    inline TLorentzVector operator()(
        const double e, const double eta, const double phi) const
        {
            // There is no mass associated with this energy... We will
            // assume that the mass is 0 and proceed as if the energy
            // is the momentum.
            const double pt = e/cosh(eta);
            return TLorentzVector(
                pt*cos(phi), pt*sin(phi), pt*sinh(eta), e);
        }
};

#endif // VBuilders_h_
