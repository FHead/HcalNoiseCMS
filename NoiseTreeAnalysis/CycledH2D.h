#ifndef CycledH2D_h_
#define CycledH2D_h_

//
// A wrapper around TH2D which implements ManagedHisto interface
// and knows how to fill the underlying root histogram in a cycle.
// Use the "CycledH2D" helper function to create instances of this
// wrapper.
//
// I. Volobouev
// March 2013
//

#include "ManagedHisto.h"
#include "TH2D.h"

//
// Wrapper class for TH2D. In the user code, do not create instances
// of this class directly, call the "CycledH2D" function instead.
//
template<class Functor1, class Functor2, class Functor3>
class CycledH2DHelper : public ManagedHisto
{
public:
    inline CycledH2DHelper(const char* name, const char* title,
                       const char* directory, const char* xlabel,
                       const char* ylabel, const char* zlabel,
                       unsigned n_x_bins, double xmin, double xmax,
                       unsigned n_y_bins, double ymin, double ymax,
                       Functor1 quantity1, Functor2 quantity2,
                       Functor3 weight)
        : histo_(new TH2D(name, title, n_x_bins, xmin, xmax,
                                       n_y_bins, ymin, ymax)),
          f1_(quantity1),
          f2_(quantity2),
          w_(weight),
          directory_(directory ? directory : "")
    {
        histo_->GetXaxis()->SetTitle(xlabel);
        histo_->GetYaxis()->SetTitle(ylabel);
        histo_->GetZaxis()->SetTitle(zlabel);
    }

    inline virtual ~CycledH2DHelper()
    {
        // Do not delete histo_ here due to the idiosyncratic
        // root object ownership conventions
    }

    inline void AutoFill() {}
    inline void CycleFill(const unsigned nCycles)
    {
        for (unsigned i=0; i<nCycles; ++i)
            histo_->Fill(f1_(i), f2_(i), w_(i));
    }
    inline void SetDirectory(TDirectory* d) {histo_->SetDirectory(d);}
    inline const std::string& GetDirectoryName() const {return directory_;}
    inline TH2D* GetRootItem() const {return histo_;}

private:
    TH2D* histo_;    
    Functor1 f1_;
    Functor2 f2_;
    Functor3 w_;
    std::string directory_;
};

//
// The CycledH2D function arguments are as follows:
//
// name       -- Object name for "root". Should be unique. It is the
//               user responsibility to ensure that it is unique among
//               all root objects created by the program.
//
// title      -- Histogram title
//
// directory  -- Directory inside the root file into which this histogram
//               will be placed
//
// xlabel     -- The label for the histogram X axis
//
// ylabel     -- The label for the histogram Y axis
//
// zlabel     -- The label for the bin counts
//
// nxbins     -- Number of X axis bins (binning will be uniform)
//
// xmin, xmax -- X axis limits
//
// nybins     -- Number of Y axis bins (binning will be uniform)
//
// ymin, ymax -- Y axis limits
//
// quantity1, quantity2  -- Functors for the quantities to histogram along
//               the X and Y axes, respectively. These functors must define
//               method "operator()(unsigned)" whose result must be convertible
//               to double. These functors will be called in a cycle with
//               argument incremented from 0 to some user-provided limit.
//
// weight     -- Functor for the bin weights (the numbers added to the bin
//               values). Must define method "operator()(unsigned)" whose
//               result must be convertible to double. Can be used to
//               implement implicit selection cuts by returning 0 or "false".
//
// This function returns a pointer to a new histogram created on the heap.
// This pointer should be managed by a HistogramManager instance (call
// its "manage" method).
//
template<class Functor1, class Functor2, class Functor3>
inline CycledH2DHelper<Functor1,Functor2,Functor3>* CycledH2D(
    const char* name, const char* title,
    const char* directory, const char* xlabel,
    const char* ylabel, const char* zlabel,
    unsigned nxbins, double xmin, double xmax,
    unsigned nybins, double ymin, double ymax,
    Functor1 quantity1, Functor2 quantity2, Functor3 weight)
{
    return new CycledH2DHelper<Functor1,Functor2,Functor3>(
        name, title, directory, xlabel, ylabel, zlabel, nxbins, xmin, xmax,
        nybins, ymin, ymax, quantity1, quantity2, weight);
}

#endif // CycledH2D_h_
