#ifndef HcalChargeFilter_h_
#define HcalChargeFilter_h_

#include <cassert>

#include "ChannelChargeInfo.h"
#include "npstat/nm/Matrix.hh"

class HcalChargeFilter
{
public:
    enum {
        nTimeSlices = ChannelChargeInfo::nTimeSlices
    };

    // Default constructor builds an invalid filter
    HcalChargeFilter();

    // Quadratic filter constructor
    HcalChargeFilter(const npstat::Matrix<double>& m,
                     const double b[nTimeSlices], double c,
                     unsigned minTS, unsigned maxTS);

    // Linear filter constructor
    HcalChargeFilter(const double b[nTimeSlices], double c,
                     unsigned minTS, unsigned maxTS);

    HcalChargeFilter(const HcalChargeFilter&);
    HcalChargeFilter& operator=(const HcalChargeFilter&);

    inline ~HcalChargeFilter() {delete a_;}

    // Check if the filter is valid
    inline bool isValid() const {return minTS_ < maxTS_;}

    // Apply the filter
    template <typename Real>
    inline double operator()(const Real *ts, const unsigned nSlices) const
    {
        assert(ts);
        assert(nSlices == nTimeSlices);
        assert(minTS_ < maxTS_);

        double sum = c_;
        for (unsigned i=minTS_; i<maxTS_; ++i)
            sum += b_[i]*ts[i];
        if (a_)
            sum += a_->bilinear(ts + minTS_, maxTS_ - minTS_);
        return sum;
    }

    // Comparison for equality
    bool operator==(const HcalChargeFilter& r) const;
    inline bool operator!=(const HcalChargeFilter& r) const
        {return !(*this == r);}

    // I/O methods needed for writing
    inline gs::ClassId classId() const {return gs::ClassId(*this);}
    bool write(std::ostream& of) const;

    // I/O methods needed for reading
    static inline const char* classname() {return "HcalChargeFilter";}
    static inline unsigned version() {return 1;}
    static void restore(const gs::ClassId& id, std::istream& in,
                        HcalChargeFilter* ptr);
private:
    const npstat::Matrix<double>* a_;
    double b_[nTimeSlices];
    double c_;
    unsigned minTS_;
    unsigned maxTS_;
};

#endif // HcalChargeFilter_h_
