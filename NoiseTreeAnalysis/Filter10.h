#ifndef Filter10_h_
#define Filter10_h_

//
// A class for applying linear filters to 10-element arrays.
// Intended for use in the analysis of HCAL pulse time structure.
//
// Think that the filter starts at time t0. Then it is applied
// to the signal "s" so that the result at time t is the sum over j
// of f[j]*s[t+j] for all j >= t0 and j < t0 + (width of the filter).
//
// For finite signals, we need to do something at the boundaries.
// Here, it is assumed that the signal for negative times equals
// s[0] and for large positive times equals s[9].
//
// If you have an array of doubles "f" and integer "t0" then
// an object of this class can be constructed as follows:
//
// Filter10 filter(f, sizeof(f)/sizeof(f[0]), t0).
//
// The following arrays illustrate some typical filters, to give
// you an idea of how this code could be used:
//
// Identity:            double f[] = {1};             int t0 = 0;
// Shift right by 1:    double f[] = {1};             int t0 = -1;
// Shift left by 1:     double f[] = {1};             int t0 = 1;
// Derivative:          double f[] = {-1, 1};         int t0 = -1;
// Second derivative:   double f[] = {1, -2, 1};      int t0 = -1;
// Width-2 low-pass:    double f[] = {1, 1};          int t0 = 0;
// Width-2 derivative:  double f[] = {-1, -1, 1, 1};  int t0 = -2;
//
// I. Volobouev
// March 2013
//

#include <vector>
#include <cassert>
#include <stdexcept>

class Filter10
{
public:
    inline Filter10(const std::vector<double>& filtData, int filterStartIndex)
        : filt_(filtData), i0_(filterStartIndex)
    {
        if (filt_.empty())
          throw std::invalid_argument("In Filter10 constructor: no filter data");
    }

    inline Filter10(const double* data, unsigned dataLen, int filterStartIndex)
        : i0_(filterStartIndex)
    {
        if (!dataLen)
          throw std::invalid_argument("In Filter10 constructor: no filter data");
        assert(data);
        std::vector<double> tmp(data, data+dataLen);
        filt_.swap(tmp);
    }

    inline const std::vector<double>& filterData() const {return filt_;}
    inline int filterStartTime() const {return i0_;}

    //
    // Both "in" and "out" should be 10-element arrays
    //
    template<class Data>
    inline void apply(const Data* in, Data* out) const
    {
        assert(in);
        assert(out);
        assert(in != out);

        const int sz = filt_.size();
        const double* f = &filt_[0];
        for (int t=0; t<10; ++t)
        {
            const int shift = t + i0_;
            double sum = 0.0;
            for (int j=0; j<sz; ++j)
            {
                register int i = j + shift;
                if (i < 0)
                    i = 0;
                else if (i > 9)
                    i = 9;
                sum += f[j]*in[i];
            }
            out[t] = static_cast<Data>(sum);
        }
    }

private:
    Filter10();

    std::vector<double> filt_;
    int i0_;
};

#endif // Filter10_h_
