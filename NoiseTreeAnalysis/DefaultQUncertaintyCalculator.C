#include <cmath>
#include <cassert>
#include <algorithm>

#include "DefaultQUncertaintyCalculator.h"
#include "ChannelChargeMix.h"

double DefaultQUncertaintyCalculator::operator()(
    const ChannelChargeMix& mix) const
{
    // A simple model for the charge uncertainty
    const double q = std::max(mix.chargeResponse, 0.f);
    const double sigma = a_*q + b_*std::sqrt(q) + c_;
    assert(sigma > 0.0);
    return sigma;
}
