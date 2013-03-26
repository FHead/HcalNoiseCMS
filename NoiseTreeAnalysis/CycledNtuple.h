#ifndef CycledNtuple_h_
#define CycledNtuple_h_

//
// Wrapper template for TNtuple and TNtupleD which implements ManagedHisto
// interface and knows how to fill the underlying ntuples automatically in
// a cycle. Use the helper functions "CycledNtuple" and "CycledNtupleD"
// to create instances of the corresponding ntuples.
//
// I. Volobouev
// March 2013
//

#include "ManagedHisto.h"
#include "NtuplePacker.h"
#include "Column.h"
#include "AllPass.h"

//
// In the user code, do not create instances of this class directly, call
// the "CycledNtuple" or "CycledNtupleD" functions instead
//
template <class NtuplePacker, class Ntuple, typename Real, typename Selector>
class CycledNtupleHelper : public ManagedHisto
{
public:
    typedef Ntuple ntuple_type;
    typedef Real real_type;

    CycledNtupleHelper(const char* name, const char* title,
                       const char* directory, const NtuplePacker& packer,
                       const Selector& selector)
        : nt_(0),
          directory_(directory),
          packer_(packer),
          sel_(selector)
    {
        const std::string& columns = getColumnsFromPacker(packer_);
        nt_ = new Ntuple(name, title, columns.c_str());
        assert(nt_->GetNvar() > 0);
        buffer_.resize(nt_->GetNvar());
    }

    inline virtual ~CycledNtupleHelper()
    {
        // Do not delete nt_ here due to the idiosyncratic
        // root object ownership conventions
    }

    inline void AutoFill() {}
    inline void CycleFill(const unsigned nCycles)
    {
        for (unsigned i=0; i<nCycles; ++i)
            if (sel_(i))
                fillNtupleWithCycledPacker(nt_, &buffer_[0], packer_, i);
    }
    inline void SetDirectory(TDirectory* d) {nt_->SetDirectory(d);}
    inline const std::string& GetDirectoryName() const {return directory_;}
    inline Ntuple* GetRootItem() const {return nt_;}

private:
    Ntuple* nt_;
    std::string directory_;
    std::vector<Real> buffer_;
    NtuplePacker packer_;
    Selector sel_;
};

//
// NtuplePacker template parameter is an std::tuple of ColumnHelper objects.
// Functors inside ColumnHelpers must implement "operator()(unsigned)" which
// returns a number convertible to float.
//
// Selector template parameter is a functor which implements
// "operator()(unsigned)" whose result is bool or can be converted to bool.
// If the result returned by this operator is "true", the ntuple will be filled.
//
// Other arguments are as follows:
//
// name       -- Object name for "root". Should be unique. It is the
//               user responsibility to ensure that it is unique among
//               all root objects created by the program.
//
// title      -- Ntuple title
//
// directory  -- Directory inside the root file into which this ntuple
//               will be placed
//
template<class NtuplePacker, class Selector>
inline CycledNtupleHelper<NtuplePacker, TNtuple, float, Selector>* CycledNtuple(
    const char* name, const char* title,
    const char* directory, const NtuplePacker& packer,
    const Selector& selector)
{
    return new CycledNtupleHelper<NtuplePacker, TNtuple, float, Selector>(
        name, title, directory, packer, selector);
}

//
// Same thing as before but without a selector. Ntuple will be filled
// every time.
//
template<class NtuplePacker>
inline CycledNtupleHelper<NtuplePacker, TNtuple, float, AllPass>* CycledNtuple(
    const char* name, const char* title,
    const char* directory, const NtuplePacker& packer)
{
    return new CycledNtupleHelper<NtuplePacker, TNtuple, float, AllPass>(
        name, title, directory, packer, AllPass());
}

//
// Same as "CycledNtuple" but creating an ntuple of doubles instead of floats
//
template<class NtuplePacker, class Selector>
inline CycledNtupleHelper<NtuplePacker, TNtupleD, double, Selector>* CycledNtupleD(
    const char* name, const char* title,
    const char* directory, const NtuplePacker& packer,
    const Selector& selector)
{
    return new CycledNtupleHelper<NtuplePacker, TNtupleD, double, Selector>(
        name, title, directory, packer, selector);
}

template<class NtuplePacker>
inline CycledNtupleHelper<NtuplePacker, TNtupleD, double, AllPass>* CycledNtupleD(
    const char* name, const char* title,
    const char* directory, const NtuplePacker& packer)
{
    return new CycledNtupleHelper<NtuplePacker, TNtupleD, double, AllPass>(
        name, title, directory, packer, AllPass());
}

#endif // CycledNtuple_h_
