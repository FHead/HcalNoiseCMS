#ifndef nta_deltaPhi_h_
#define nta_deltaPhi_h_

//
// Simple function for calculating delta in phi.
//
// Surely, identically named functions already exist (for example,
// in the "rk" package), so this gets into a namespace.
//
// I. Volobouev
// April 2013
//

#include <cmath>

namespace nta {
    template <typename V1, typename V2>
    double deltaPhi(const V1& v1, const V2& v2);

    template <>
    inline double deltaPhi<double, double>(const double& phi1,
                                           const double& phi2)
    {
        double delta = phi1 - phi2;
        while (delta > M_PI)
            delta -= 2.0*M_PI;
        while (delta < -M_PI)
            delta += 2.0*M_PI;
        return delta;
    }

    template <>
    inline double deltaPhi<double, float>(const double& phi1,
                                          const float& phi2)
    {
        return deltaPhi(phi1, static_cast<double>(phi2));
    }

    template <>
    inline double deltaPhi<float, double>(const float& phi1,
                                          const double& phi2)
    {
        return deltaPhi(static_cast<double>(phi1), phi2);
    }

    template <>
    inline double deltaPhi<float, float>(const float& phi1,
                                         const float& phi2)
    {
        return deltaPhi(static_cast<double>(phi1), static_cast<double>(phi2));
    }

    template <typename V1, typename V2>
    inline double deltaPhi(const V1& v1, const V2& v2)
    {
        return deltaPhi(v1.Phi(), v2.Phi());
    }
}

#endif // nta_deltaPhi_h_
