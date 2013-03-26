#ifndef AutoH1D_h_
#define AutoH1D_h_

//
// A wrapper around TH1D which implements ManagedHisto interface
// and knows how to fill the underlying root histogram. Use the
// "AutoH1D" helper function to create instances of this wrapper.
//
// I. Volobouev
// March 2013
//

#include "ManagedHisto.h"
#include "TH1D.h"

//
// Wrapper class for TH1D. In the user code, do not create instances
// of this class directly, call the "AutoH1D" function instead.
//
template<class Functor1, class Functor2>
class AutoH1DHelper : public ManagedHisto
{
public:
    inline AutoH1DHelper(const char* name, const char* title,
                         const char* directory,
                         const char* xlabel, const char* ylabel,
                         unsigned nbins, double xmin, double xmax,
                         Functor1 quantity, Functor2 weight)
        : histo_(new TH1D(name, title, nbins, xmin, xmax)),
          f_(quantity),
          w_(weight),
          directory_(directory ? directory : "")
    {
        histo_->GetXaxis()->SetTitle(xlabel);
        histo_->GetYaxis()->SetTitle(ylabel);
    }

    inline virtual ~AutoH1DHelper()
    {
        // Do not delete histo_ here due to the idiosyncratic
        // root object ownership conventions
    }

    inline void AutoFill() {histo_->Fill(f_(), w_());}
    inline void CycleFill(unsigned) {}
    inline void SetDirectory(TDirectory* d) {histo_->SetDirectory(d);}
    inline const std::string& GetDirectoryName() const {return directory_;}
    inline TH1D* GetRootItem() const {return histo_;}

private:
    TH1D* histo_;
    Functor1 f_;
    Functor2 w_;
    std::string directory_;
};

//
// The AutoH1D function arguments are as follows:
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
// xlabel     -- The label for the horizontal axis
//
// ylabel     -- The label for the bin counts
//
// nbins      -- Number of horizontal axis bins (binning will be uniform)
//
// xmin, xmax -- Horizontal axis limits
//
// quantity   -- Functor for the quantity to histogram. Must define method
//               "operator()()" whose result must be convertible to double.
//
// weight     -- Functor for the bin weights (the numbers added to the bin
//               values). Must define method "operator()()" whose result
//               must be convertible to double. Can be used to implement
//               implicit selection cuts by returning 0 or "false".
//
// This function returns a pointer to a new histogram created on the heap.
// This pointer should be managed by a HistogramManager instance (call
// its "manage" method).
//
template<class Functor1, class Functor2>
inline AutoH1DHelper<Functor1,Functor2>* AutoH1D(
    const char* name, const char* title,
    const char* directory,
    const char* xlabel, const char* ylabel,
    unsigned nbins, double xmin, double xmax,
    Functor1 quantity, Functor2 weight)
{
    return new AutoH1DHelper<Functor1,Functor2>(
        name, title, directory, xlabel, ylabel,
        nbins, xmin, xmax, quantity, weight);
}

#endif // AutoH1D_h_
