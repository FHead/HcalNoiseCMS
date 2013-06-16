#ifndef DefaultQUncertaintyCalculator_h_
#define DefaultQUncertaintyCalculator_h_

//
// A simple uncertainty model for the channel charge:
// sigma(Q) = a*Q + b*sqrt(Q) + c  (Q < 0 is replaced by Q = 0).
//
// I. Volobouev
// June 2013
//

#include "AbsQUncertaintyCalculator.h"

class DefaultQUncertaintyCalculator : public AbsQUncertaintyCalculator
{
public:
    inline DefaultQUncertaintyCalculator(const double a, const double b,
                                         const double c)
        : a_(a), b_(b), c_(c) {}

    inline virtual ~DefaultQUncertaintyCalculator() {}

    virtual double operator()(const ChannelChargeMix& mix) const;

private:
    double a_;
    double b_;
    double c_;
};

#endif // DefaultQUncertaintyCalculator_h_
