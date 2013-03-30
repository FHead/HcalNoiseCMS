#ifndef Functors_h_
#define Functors_h_

//
// A variety of functors for use with histogram and ntuple wrappers.
// No matter what their actual type is, the values returned by operator()
// will be typically converted into either a double (histogram coordinates,
// histogram weights, ntuple values) or a boolean (ntuple selectors).
//
// Functors which implement "operator()(unsigned) const" but not
// "operator()() const" are intended for use with "Cycled" histograms
// and ntuples only. Functors which implement both of these methods
// can also be used with "Auto" histograms and ntuples.
//
// Functor templates are split into a helper class and a convenience
// function which returns an instance of that class, similar to what
// the standard library does with std::pair and std::make_pair. The
// intent is that the user code should use the helper functions only.
//
// I. Volobouev
// March 2013
//

#include <cassert>


class Double
{
public:
    inline Double(const double v) : value_(v) {}
    inline double operator()() const {return value_;}
    inline double operator()(unsigned) const {return value_;}

private:
    Double();
    double value_;
};

//======================================================================

class Boolean
{
public:
    inline Boolean(const bool v) : value_(v) {}
    inline bool operator()() const {return value_;}
    inline bool operator()(unsigned) const {return value_;}

private:
    Boolean();
    bool value_;
};

//======================================================================

template<class T>
class ConstantHlp
{
public:
    inline ConstantHlp(const T& v) : value_(v) {}
    inline const T& operator()() const {return value_;}
    inline const T& operator()(unsigned) const {return value_;}

private:
    ConstantHlp();
    T value_;
};

template<typename T>
inline ConstantHlp<T> Constant(const T& t)
{
    return ConstantHlp<T>(t);
}

//======================================================================

class UIntRatio
{
public:
    inline UIntRatio(const unsigned divisor) : divisor_(divisor) {}

    inline unsigned operator()(const unsigned n) const
        {return n/divisor_;}

private:
    unsigned divisor_;
};

//======================================================================

class UIntRemainder
{
public:
    inline UIntRemainder(const unsigned divisor) : divisor_(divisor) {}

    inline unsigned operator()(const unsigned n) const
        {return n%divisor_;}

private:
    unsigned divisor_;
};

//======================================================================

template<typename T>
class ValueOfHlp
{
public:
    inline ValueOfHlp(T& t) : ptr_(&t) {}
    inline const T& operator()() const {return *ptr_;}
    inline const T& operator()(unsigned) const {return *ptr_;}

private:
    T* ptr_;
};

template<typename T>
inline ValueOfHlp<T> ValueOf(T& t)
{
    return ValueOfHlp<T>(t);
}

//======================================================================

template<typename T>
class ValueEQHlp
{
public:
    inline ValueEQHlp(T& t, const T& v) : ptr_(&t), value_(v) {}
    inline bool operator()() const {return *ptr_ == value_;}
    inline bool operator()(unsigned) const {return *ptr_ == value_;}

private:
    T* ptr_;
    T value_;
};

template<typename T>
inline ValueEQHlp<T> ValueEQ(T& t, const T& v)
{
    return ValueEQHlp<T>(t, v);
}

//======================================================================

template<typename T>
class ValueLTHlp
{
public:
    inline ValueLTHlp(T& t, const T& v) : ptr_(&t), value_(v) {}
    inline bool operator()() const {return *ptr_ < value_;}
    inline bool operator()(unsigned) const {return *ptr_ < value_;}

private:
    T* ptr_;
    T value_;
};

template<typename T>
inline ValueLTHlp<T> ValueLT(T& t, const T& v)
{
    return ValueLTHlp<T>(t, v);
}

//======================================================================

template<typename T>
class ValueLEHlp
{
public:
    inline ValueLEHlp(T& t, const T& v) : ptr_(&t), value_(v) {}
    inline bool operator()() const {return *ptr_ <= value_;}
    inline bool operator()(unsigned) const {return *ptr_ <= value_;}

private:
    T* ptr_;
    T value_;
};

template<typename T>
inline ValueLEHlp<T> ValueLE(T& t, const T& v)
{
    return ValueLEHlp<T>(t, v);
}

//======================================================================

template<typename T>
class ValueGTHlp
{
public:
    inline ValueGTHlp(T& t, const T& v) : ptr_(&t), value_(v) {}
    inline bool operator()() const {return *ptr_ > value_;}
    inline bool operator()(unsigned) const {return *ptr_ > value_;}

private:
    T* ptr_;
    T value_;
};

template<typename T>
inline ValueGTHlp<T> ValueGT(T& t, const T& v)
{
    return ValueGTHlp<T>(t, v);
}

//======================================================================

template<typename T>
class ValueGEHlp
{
public:
    inline ValueGEHlp(T& t, const T& v) : ptr_(&t), value_(v) {}
    inline bool operator()() const {return *ptr_ >= value_;}
    inline bool operator()(unsigned) const {return *ptr_ >= value_;}

private:
    T* ptr_;
    T value_;
};

template<typename T>
inline ValueGEHlp<T> ValueGE(T& t, const T& v)
{
    return ValueGEHlp<T>(t, v);
}

//======================================================================

struct CycleNumber
{
    inline unsigned operator()(const unsigned i) const {return i;}
};

//======================================================================

template<typename T>
class ElementOfHlp
{
public:
    inline ElementOfHlp(T* t, const unsigned stride)
        : ptr_(t), stride_(stride) {assert(ptr_);}
    inline const T& operator()(const unsigned i) const
        {return ptr_[i*stride_];}

private:
    T* ptr_;
    unsigned stride_;
};

template<typename T>
inline ElementOfHlp<T> ElementOf(T* t, unsigned stride=1)
{
    return ElementOfHlp<T>(t, stride);
}

//======================================================================

template<typename T>
class ElementEQHlp
{
public:
    inline ElementEQHlp(T* t, const T& value, const unsigned stride)
        : ptr_(t), value_(value), stride_(stride) {assert(ptr_);}
    inline bool operator()(const unsigned i) const
        {return ptr_[i*stride_] == value_;}

private:
    T* ptr_;
    T value_;
    unsigned stride_;
};

template<typename T>
inline ElementEQHlp<T> ElementEQ(T* t, const T& value, unsigned stride=1)
{
    return ElementEQHlp<T>(t, value, stride);
}

//======================================================================

template<typename T>
class ElementLTHlp
{
public:
    inline ElementLTHlp(T* t, const T& value, const unsigned stride)
        : ptr_(t), value_(value), stride_(stride) {assert(ptr_);}
    inline bool operator()(const unsigned i) const
        {return ptr_[i*stride_] < value_;}

private:
    T* ptr_;
    T value_;
    unsigned stride_;
};

template<typename T>
inline ElementLTHlp<T> ElementLT(T* t, const T& value, unsigned stride=1)
{
    return ElementLTHlp<T>(t, value, stride);
}

//======================================================================

template<typename T>
class ElementLEHlp
{
public:
    inline ElementLEHlp(T* t, const T& value, const unsigned stride)
        : ptr_(t), value_(value), stride_(stride) {assert(ptr_);}
    inline bool operator()(const unsigned i) const
        {return ptr_[i*stride_] <= value_;}

private:
    T* ptr_;
    T value_;
    unsigned stride_;
};

template<typename T>
inline ElementLEHlp<T> ElementLE(T* t, const T& value, unsigned stride=1)
{
    return ElementLEHlp<T>(t, value, stride);
}

//======================================================================

template<typename T>
class ElementGTHlp
{
public:
    inline ElementGTHlp(T* t, const T& value, const unsigned stride)
        : ptr_(t), value_(value), stride_(stride) {assert(ptr_);}
    inline bool operator()(const unsigned i) const
        {return ptr_[i*stride_] > value_;}

private:
    T* ptr_;
    T value_;
    unsigned stride_;
};

template<typename T>
inline ElementGTHlp<T> ElementGT(T* t, const T& value, unsigned stride=1)
{
    return ElementGTHlp<T>(t, value, stride);
}

//======================================================================

template<typename T>
class ElementGEHlp
{
public:
    inline ElementGEHlp(T* t, const T& value, const unsigned stride)
        : ptr_(t), value_(value), stride_(stride) {assert(ptr_);}
    inline bool operator()(const unsigned i) const
        {return ptr_[i*stride_] >= value_;}

private:
    T* ptr_;
    T value_;
    unsigned stride_;
};

template<typename T>
inline ElementGEHlp<T> ElementGE(T* t, const T& value, unsigned stride=1)
{
    return ElementGEHlp<T>(t, value, stride);
}

//======================================================================

template<typename Result, typename T>
class ElementMemberFcnHlp0
{
public:
    typedef Result (T::*MemFunc)();

    inline ElementMemberFcnHlp0(MemFunc f, T* obj, const unsigned stride)
        : f_(f), ptr_(obj), stride_(stride) {assert(ptr_);}

    inline Result operator()(const unsigned i) const
        {return ((ptr_ + i*stride_)->*f_)();}

private:
    MemFunc f_;
    T* ptr_;
    unsigned stride_;
};

template<typename Result, typename T>
inline ElementMemberFcnHlp0<Result,T> ElementMethod(
    Result (T::*f)(), T* ptr, unsigned stride=1)
{
    return ElementMemberFcnHlp0<Result,T>(f, ptr, stride);
}

//======================================================================

template<typename Result, typename T>
class ElementMemberFcnHlp0Const
{
public:
    typedef Result (T::*MemFunc)() const;

    inline ElementMemberFcnHlp0Const(MemFunc f, const T* obj,
                                     const unsigned stride)
        : f_(f), ptr_(obj), stride_(stride) {assert(ptr_);}

    inline Result operator()(const unsigned i) const
        {return ((ptr_ + i*stride_)->*f_)();}

private:
    MemFunc f_;
    const T* ptr_;
    unsigned stride_;
};

template<typename Result, typename T>
inline ElementMemberFcnHlp0Const<Result,T> ElementMethod(
    Result (T::*f)() const, const T* ptr, unsigned stride=1)
{
    return ElementMemberFcnHlp0Const<Result,T>(f, ptr, stride);
}

//======================================================================

template<typename Result, typename T>
class ElementMemberHlp
{
public:
    inline ElementMemberHlp(const T* arr, const unsigned long offset,
                            const unsigned stride)
        : ptr_(arr), offset_(offset), stride_(stride) {}

    inline Result operator()(const unsigned i) const
    {
        return *(reinterpret_cast<const Result*>(
                 reinterpret_cast<const char*>(ptr_+i*stride_)+offset_));
    }

private:
    const T* ptr_;
    unsigned long offset_;
    unsigned stride_;
};


template<typename Result, typename T>
inline ElementMemberHlp<Result,T> ElementMember(
    const T* base, const Result* element, unsigned stride=1)
{
    assert(base);
    assert(element);
    long off = reinterpret_cast<const char*>(element) - 
               reinterpret_cast<const char*>(base);
    assert(off >= 0L);
    return ElementMemberHlp<Result,T>(base, off, stride);
}

//======================================================================

template<typename T>
class CMathFcnHlp
{
public:
    typedef double (*CFunc)(double);

    inline CMathFcnHlp(CFunc f, T& t1) : fcn_(f), ptr1_(&t1) {}
    inline double operator()() const {return fcn_(*ptr1_);}

private:
    CFunc fcn_;
    T* ptr1_;
};

template<typename T>
inline CMathFcnHlp<T> Apply(double (*f)(double), T& t)
{
    return CMathFcnHlp<T>(f, t);
}

//======================================================================

template<typename T>
class BinaryFcnHlp
{
public:
    typedef double (*CFunc)(double, double);

    inline BinaryFcnHlp(CFunc f, T& t1, T& t2)
        : fcn_(f), ptr1_(&t1), ptr2_(&t2) {}
    inline double operator()() const {return fcn_(*ptr1_, *ptr2_);}

private:
    CFunc fcn_;
    T* ptr1_;
    T* ptr2_;
};


template<typename T>
inline BinaryFcnHlp<T> Apply(double (*f)(double, double), T& t1, T& t2)
{
    return BinaryFcnHlp<T>(f, t1, t2);
}

//======================================================================

template<typename Result, typename T>
class MemberFcnHlp0
{
public:
    typedef Result (T::*MemFunc)();

    inline MemberFcnHlp0(MemFunc f, T* obj)
        : f_(f), ptr_(obj) {assert(ptr_);}
    inline Result operator()() const {return (ptr_->*f_)();}
    inline Result operator()(unsigned) const {return (ptr_->*f_)();}

private:
    MemFunc f_;
    T* ptr_;
};

template<typename Result, typename T>
inline MemberFcnHlp0<Result,T> Method(Result (T::*f)(), T* ptr)
{
    return MemberFcnHlp0<Result,T>(f, ptr);
}

//======================================================================

template<typename Result, typename T>
class MemberFcnHlp0Const
{
public:
    typedef Result (T::*MemFunc)() const;

    inline MemberFcnHlp0Const(MemFunc f, const T* obj)
        : f_(f), ptr_(obj) {assert(ptr_);}
    inline Result operator()() const {return (ptr_->*f_)();}
    inline Result operator()(unsigned) const {return (ptr_->*f_)();}

private:
    MemFunc f_;
    const T* ptr_;
};

template<typename Result, typename T>
inline MemberFcnHlp0Const<Result,T> Method(Result (T::*f)() const,
                                           const T* ptr)
{
    return MemberFcnHlp0Const<Result,T>(f, ptr);
}

//======================================================================

template<typename Result, typename T>
class MemberFcnHlp1
{
public:
    typedef Result (T::*MemFunc)(unsigned);

    inline MemberFcnHlp1(MemFunc f, T* obj)
        : f_(f), ptr_(obj) {assert(ptr_);}
    inline Result operator()(unsigned n) const {return (ptr_->*f_)(n);}

private:
    MemFunc f_;
    T* ptr_;
};

template<typename Result, typename T>
inline MemberFcnHlp1<Result,T> Method(Result (T::*f)(unsigned), T* ptr)
{
    return MemberFcnHlp1<Result,T>(f, ptr);
}

//======================================================================

template<typename Result, typename T>
class MemberFcnHlp1Const
{
public:
    typedef Result (T::*MemFunc)(unsigned) const;

    inline MemberFcnHlp1Const(MemFunc f, const T* obj)
        : f_(f), ptr_(obj) {assert(ptr_);}
    inline Result operator()(unsigned n) const {return (ptr_->*f_)(n);}

private:
    MemFunc f_;
    const T* ptr_;
};

template<typename Result, typename T>
inline MemberFcnHlp1Const<Result,T> Method(Result (T::*f)(unsigned) const,
                                           const T* ptr)
{
    return MemberFcnHlp1Const<Result,T>(f, ptr);
}

#endif // Functors_h_
