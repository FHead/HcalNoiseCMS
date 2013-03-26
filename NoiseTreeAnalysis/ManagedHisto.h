#ifndef MANAGEDHISTO_H_
#define MANAGEDHISTO_H_

//
// To work properly with the HistogramManager class, managed histogram
// and ntuple wrappers should implement the "ManagedHisto" interface
//
// I. Volobouev
// March 2013
//

#include <string>
#include <cstring>
#include <vector>
#include <cassert>

#include "TDirectory.h"


struct ManagedHisto
{
    inline virtual ~ManagedHisto() {}

    // To avoid potential confusion, derived classes should have
    // a meaningful implementation of either AutoFill or CycleFill
    // but not both. The other function in this pair should be
    // implemented as a NOOP.
    virtual void AutoFill() = 0;
    virtual void CycleFill(unsigned nCycles) = 0;

    virtual void SetDirectory(TDirectory* d) = 0;
    virtual const std::string& GetDirectoryName() const = 0;
    virtual TObject* GetRootItem() const = 0;
};


struct ManagedHistoContainer : public std::vector<ManagedHisto*>
{
    inline ManagedHistoContainer() : std::vector<ManagedHisto*>() {}

    inline ~ManagedHistoContainer()
    {
        const std::size_t n = this->size();
        for (std::size_t i=0; i<n; ++i)
            delete (*this)[i];
    }

    inline void AutoFill()
    {
        const std::size_t n = this->size();
        for (std::size_t i=0; i<n; ++i)
            (*this)[i]->AutoFill();
    }

    inline void CycleFill(const unsigned nCycles)
    {
        const std::size_t n = this->size();
        for (std::size_t i=0; i<n; ++i)
            (*this)[i]->CycleFill(nCycles);
    }

    inline TObject* LastRootItem() const
    {
        TObject* obj = 0;
        const std::size_t n = this->size();
        if (n)
            obj = (*this)[n - 1]->GetRootItem();
        return obj;
    }

    inline TObject* FindByName(const char* name) const
    {
        assert(name);
        TObject* obj = 0;
        const std::size_t n = this->size();
        for (std::size_t i=0; i<n; ++i)
        {
            TNamed* no = dynamic_cast<TNamed*>((*this)[i]->GetRootItem());
            if (no)
                if (strcmp(no->GetName(), name) == 0)
                {
                    obj = no;
                    break;
                }
        }
        return obj;
    }
};

#endif // MANAGEDHISTO_H_
