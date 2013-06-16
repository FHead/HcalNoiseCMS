#include "HcalChargeFilter.h"

#include "geners/binaryIO.hh"
#include "geners/IOException.hh"

using namespace gs;

HcalChargeFilter::HcalChargeFilter()
    : a_(0), c_(0.0), minTS_(0U), maxTS_(0U)
{
}

HcalChargeFilter::HcalChargeFilter(const HcalChargeFilter& r)
    : a_(0), c_(r.c_), minTS_(r.minTS_), maxTS_(r.maxTS_)
{
    if (r.a_)
        a_ = new npstat::Matrix<double>(*r.a_);
    for (unsigned i=0; i<nTimeSlices; ++i)
        b_[i] = r.b_[i];
}

HcalChargeFilter& HcalChargeFilter::operator=(const HcalChargeFilter& r)
{
    if (this != &r)
    {
        delete a_; a_ = 0;
        if (r.a_)
            a_ = new npstat::Matrix<double>(*r.a_);
        for (unsigned i=0; i<nTimeSlices; ++i)
            b_[i] = r.b_[i];
        c_ = r.c_;
        minTS_ = r.minTS_;
        maxTS_ = r.maxTS_;
    }
    return *this;
}

HcalChargeFilter::HcalChargeFilter(const npstat::Matrix<double>& m,
                                   const double b[nTimeSlices], const double c,
                                   const unsigned minTS, const unsigned maxTS)
    : a_(0), c_(c), minTS_(minTS), maxTS_(maxTS)
{
    assert(m.nRows() == nTimeSlices);
    assert(m.nColumns() == nTimeSlices);
    assert(minTS_ < maxTS_);
    assert(maxTS_ <= nTimeSlices);
    const unsigned nTS = maxTS_ - minTS_;
    a_ = new npstat::Matrix<double>(nTS, nTS);
    for (unsigned i=0; i<nTS; ++i)
        for (unsigned j=0; j<nTS; ++j)
            (*a_)[i][j] = m[i+minTS][j+minTS];
    for (unsigned i=0; i<nTimeSlices; ++i)
        b_[i] = b[i];
}

HcalChargeFilter::HcalChargeFilter(const double b[nTimeSlices], const double c,
                                   const unsigned minTS, const unsigned maxTS)
    : a_(0), c_(c), minTS_(minTS), maxTS_(maxTS)
{
    assert(minTS_ < maxTS_);
    assert(maxTS_ <= nTimeSlices);
    for (unsigned i=0; i<nTimeSlices; ++i)
        b_[i] = b[i];
}

bool HcalChargeFilter::operator==(const HcalChargeFilter& r) const
{
    if (!(c_ == r.c_ &&
          minTS_ == r.minTS_ &&
          maxTS_ == r.maxTS_))
        return false;

    for (unsigned i=minTS_; i<maxTS_; ++i)
        if (b_[i] != r.b_[i])
            return false;

    if (a_ && r.a_)
        return *a_ == *r.a_;
    else if (!a_ && !r.a_)
        return true;
    else
        return false;
}

bool HcalChargeFilter::write(std::ostream& of) const
{
    write_pod_array(of, b_, nTimeSlices);
    write_pod(of, c_);
    write_pod(of, minTS_);
    write_pod(of, maxTS_);
    const bool hasMatrix = a_;
    const char h = hasMatrix;
    write_pod(of, h);
    if (hasMatrix)
        return a_->classId().write(of) && a_->write(of);
    else
        return !of.fail();
}

void HcalChargeFilter::restore(const gs::ClassId& id, std::istream& in,
                               HcalChargeFilter* ptr)
{
    static const ClassId myClassId(ClassId::makeId<HcalChargeFilter>());
    myClassId.ensureSameId(id);

    assert(ptr);
    delete ptr->a_; ptr->a_ = 0;
    read_pod_array(in, ptr->b_, nTimeSlices);
    read_pod(in, &ptr->c_);
    read_pod(in, &ptr->minTS_);
    read_pod(in, &ptr->maxTS_);
    char h;
    read_pod(in, &h);
    if (in.fail()) throw IOReadFailure("In HcalChargeFilter::restore: "
                                       "input stream failure");
    if (h)
    {
        ClassId mId(in, 1);
        npstat::Matrix<double> mat;
        npstat::Matrix<double>::restore(mId, in, &mat);
        ptr->a_ = new npstat::Matrix<double>(mat);
    }
}
