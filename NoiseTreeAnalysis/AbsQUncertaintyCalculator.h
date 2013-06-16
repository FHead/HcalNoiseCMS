#ifndef ABSQUNCERTAINTYCALCULATOR_HH_
#define ABSQUNCERTAINTYCALCULATOR_HH_

class ChannelChargeMix;

struct AbsQUncertaintyCalculator
{
    inline virtual ~AbsQUncertaintyCalculator() {}

    //
    // The "chargeUncertainty" field does not have to be set to anything
    // reasonable in the "mix" argument (typically it will be just 1.0)
    //
    virtual double operator()(const ChannelChargeMix& mix) const = 0;
};

#endif // ABSQUNCERTAINTYCALCULATOR_HH_
