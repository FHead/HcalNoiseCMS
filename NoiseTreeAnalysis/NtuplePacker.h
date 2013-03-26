#ifndef NTUPLEPACKER_H_
#define NTUPLEPACKER_H_

//
// Helper classes and functions for generating TNtuple definitions
// from an initial specification defined by std::tuple of columns
//
// I. Volobouev
// March 2013
//

#include <tuple>
#include <string>
#include <cassert>

#include "TNtuple.h"
#include "TNtupleD.h"

namespace tupleutils
{
    template<typename Tuple, int N>
    struct TupleNameCycler
    {
        inline static void collect(const Tuple& t, std::string* s)
        {
            TupleNameCycler<Tuple, N-1>::collect(t, s);
            if (N-1)
                *s += ":";
            *s += (std::get<N-1>(t)).name;
        }
    };

    template<typename Tuple>
    struct TupleNameCycler<Tuple, 0>
    {
        inline static void collect(const Tuple& t, std::string* s)
        {assert(s); s->clear();}
    };

    template<typename Pack, typename Real, int N>
    struct TupleFillCycler
    {
        inline static void cycle(const Pack& p, Real* buffer)
        {
            TupleFillCycler<Pack, Real, N-1>::cycle(p, buffer);
            buffer[N-1] = (std::get<N-1>(p)).fcn();
        }
    };

    template<typename Pack, typename Real>
    struct TupleFillCycler<Pack, Real, 0>
    {
        inline static void cycle(const Pack&, Real* buffer)
            {assert(buffer);}
    };

    template<typename Pack, typename Real, int N>
    struct TupleFillCycleC
    {
        inline static void cycle(const Pack& p, Real* buffer, const unsigned i)
        {
            TupleFillCycleC<Pack, Real, N-1>::cycle(p, buffer, i);
            buffer[N-1] = (std::get<N-1>(p)).fcn(i);
        }
    };

    template<typename Pack, typename Real>
    struct TupleFillCycleC<Pack, Real, 0>
    {
        inline static void cycle(const Pack&, Real* buffer, unsigned)
            {assert(buffer);}
    };
}

template <class Pack>
inline std::string getColumnsFromPacker(const Pack& pack)
{
    std::string s;
    tupleutils::TupleNameCycler<Pack,std::tuple_size<Pack>::value>::collect(
        pack, &s);
    return s;
}

template <class Pack>
inline void fillNtupleWithPacker(TNtupleD* nt, double* buffer, Pack& pack)
{
    tupleutils::TupleFillCycler<Pack,double,std::tuple_size<Pack>::value>::cycle(
        pack, buffer);
    nt->Fill(buffer);
}

template <class Pack>
inline void fillNtupleWithPacker(TNtuple* nt, float* buffer, Pack& pack)
{
    tupleutils::TupleFillCycler<Pack,float,std::tuple_size<Pack>::value>::cycle(
        pack, buffer);
    nt->Fill(buffer);
}

template <class Pack>
inline void fillNtupleWithCycledPacker(TNtupleD* nt, double* buffer, Pack& pack,
                                       const unsigned index)
{
    tupleutils::TupleFillCycleC<Pack,double,std::tuple_size<Pack>::value>::cycle(
        pack, buffer, index);
    nt->Fill(buffer);
}

template <class Pack>
inline void fillNtupleWithCycledPacker(TNtuple* nt, float* buffer, Pack& pack,
                                       const unsigned index)
{
    tupleutils::TupleFillCycleC<Pack,float,std::tuple_size<Pack>::value>::cycle(
        pack, buffer, index);
    nt->Fill(buffer);
}

#endif // NTUPLEPACKER_H_
