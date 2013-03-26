#ifndef ntupleUtils_h_
#define ntupleUtils_h_

//
// Helper classes and functions for processing TNtuple/TNtupleD contents
//
// I. Volobouev
// March 2013
//

#include <cassert>

class TNtupleD;

namespace Private {
    template<class Tuple>
    struct TypeContentType
    {
        typedef float type;
    };

    template<>
    struct TypeContentType<class TNtupleD>
    {
        typedef double type;
    };
}

template<class Tuple>
class ColumnValueHlp
{
public:
    typedef typename Private::TypeContentType<Tuple>::type content_type;

    inline ColumnValueHlp(Tuple* t, unsigned col)
        : t_(t), col_(col)
    {
        assert(t_);
        assert(col_ < static_cast<unsigned>(t_->GetNvar()));
    }

    inline content_type operator()() const {return t_->GetArgs()[col_];}

private:
    Tuple* t_;
    unsigned col_;
};

template<class Tuple>
inline ColumnValueHlp<Tuple> ColumnValue(Tuple* t, unsigned col)
{
    return ColumnValueHlp<Tuple>(t, col);
}

#endif // ntupleUtils_h_
