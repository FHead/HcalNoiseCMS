#ifndef INPUTVALIDATION_HH_
#define INPUTVALIDATION_HH_

//
// Generic validation for input parameters. The functions below
// check that the parameter satisfies some condition and throw
// std::invalid_argument if it does not.
//
// I. Volobouev
// May 2013
//

#include <cassert>
#include <sstream>
#include <stdexcept>

template<class T>
void validateRangeLELT(const T& input, const char* name,
                       const T& lolim, const T& hilim)
{
    if (!(lolim <= input && input < hilim))
    {
        assert(name);
        std::ostringstream os;
        os << "In validateRangeLELT: \"" << name << "\" value "
           << input << " is outside of allowed range [" << lolim
           << ", " << hilim << ')';
        throw std::invalid_argument(os.str());
    }
}

template<class T>
void validateRangeLTLE(const T& input, const char* name,
                       const T& lolim, const T& hilim)
{
    if (!(lolim < input && input <= hilim))
    {
        assert(name);
        std::ostringstream os;
        os << "In validateRangeLTLE: \"" << name << "\" value "
           << input << " is outside of allowed range (" << lolim
           << ", " << hilim << ']';
        throw std::invalid_argument(os.str());
    }
}

template<class T>
void validateRangeLELE(const T& input, const char* name,
                       const T& lolim, const T& hilim)
{
    if (!(lolim <= input && input <= hilim))
    {
        assert(name);
        std::ostringstream os;
        os << "In validateRangeLELE: \"" << name << "\" value "
           << input << " is outside of allowed range [" << lolim
           << ", " << hilim << ']';
        throw std::invalid_argument(os.str());
    }
}

template<class T>
void validateRangeLTLT(const T& input, const char* name,
                       const T& lolim, const T& hilim)
{
    if (!(lolim < input && input < hilim))
    {
        assert(name);
        std::ostringstream os;
        os << "In validateRangeLTLT: \"" << name << "\" value "
           << input << " is outside of allowed range (" << lolim
           << ", " << hilim << ')';
        throw std::invalid_argument(os.str());
    }
}

#endif // INPUTVALIDATION_HH_
