#ifndef AutoH3D_h_
#define AutoH3D_h_

//
// A wrapper around TH3D which implements ManagedHisto interface
// and knows how to fill the underlying root histogram. Use the
// "AutoH3D" helper function to create instances of this wrapper.
//
// I. Volobouev
// March 2013
//

#include "ManagedHisto.h"
#include "TH3D.h"

//
// Wrapper class for TH3D. In the user code, do not create instances
// of this class directly, call the "AutoH3D" function instead.
//
template<class Functor1, class Functor2, class Functor3, class Functor4>
class AutoH3DHelper : public ManagedHisto
{
public:
    inline AutoH3DHelper(const char* name, const char* title,
                         const char* directory, const char* xlabel,
                         const char* ylabel, const char* zlabel,
                         const char* /* tlabel */,
                         unsigned n_x_bins, double xmin, double xmax,
                         unsigned n_y_bins, double ymin, double ymax,
                         unsigned n_z_bins, double zmin, double zmax,
                         Functor1 quantity1, Functor2 quantity2,
                         Functor3 quantity3, Functor4 weight)
        : histo_(new TH3D(name, title, n_x_bins, xmin, xmax,
                                       n_y_bins, ymin, ymax,
                                       n_z_bins, zmin, zmax)),
          f1_(quantity1),
          f2_(quantity2),
          f3_(quantity3),
          w_(weight),
          directory_(directory ? directory : "")
    {
        histo_->GetXaxis()->SetTitle(xlabel);
        histo_->GetYaxis()->SetTitle(ylabel);
        histo_->GetZaxis()->SetTitle(zlabel);

        // There is no method to get the 4th axis. Apparently,
        // 3-d histogramming was an afterthought in root, so the
        // fat interface of TH1 is not fat enought. Nevertheless,
        // to ensure API consistency, we will keep the 4th label
        // argument in the constructor.
    }

    inline virtual ~AutoH3DHelper()
    {
        // Do not delete histo_ here due to the idiosyncratic
        // root object ownership conventions
    }

    inline void AutoFill() {histo_->Fill(f1_(), f2_(), f3_(), w_());}
    inline void CycleFill(unsigned) {}
    inline void SetDirectory(TDirectory* d) {histo_->SetDirectory(d);}
    inline const std::string& GetDirectoryName() const {return directory_;}
    inline TH3D* GetRootItem() const {return histo_;}

private:
    TH3D* histo_;    
    Functor1 f1_;
    Functor2 f2_;
    Functor3 f3_;
    Functor4 w_;
    std::string directory_;
};

//
// The AutoH3D function arguments are as follows:
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
// zlabel     -- The label for the histogram Z axis
//
// tlabel     -- The label for the bin counts (currently unused -- root does
//               not know how to set a label on bin counts for 3-d histograms)
//
// nxbins     -- Number of X axis bins (binning will be uniform)
//
// xmin, xmax -- X axis limits
//
// nybins     -- Number of Y axis bins (binning will be uniform)
//
// ymin, ymax -- Y axis limits
//
// nzbins     -- Number of Z axis bins (binning will be uniform)
//
// zmin, zmax -- Z axis limits
//
// quantity1, quantity2, quantity3 -- Functors for the quantities to histogram
//               along the X, Y and Z axes, respectively. These functors must
//               define method "operator()()" whose result must be convertible
//               to double.
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
template<class Functor1, class Functor2, class Functor3, class Functor4>
inline AutoH3DHelper<Functor1,Functor2,Functor3,Functor4>* AutoH3D(
    const char* name, const char* title,
    const char* directory, const char* xlabel,
    const char* ylabel, const char* zlabel,
    const char* tlabel,
    unsigned nxbins, double xmin, double xmax,
    unsigned nybins, double ymin, double ymax,
    unsigned nzbins, double zmin, double zmax,
    Functor1 quantity1, Functor2 quantity2, Functor3 quantity3, Functor4 weight)
{
    return new AutoH3DHelper<Functor1,Functor2,Functor3,Functor4>(
        name, title, directory, xlabel, ylabel, zlabel, tlabel,
        nxbins, xmin, xmax, nybins, ymin, ymax,
        nzbins, zmin, zmax, quantity1, quantity2, quantity3, weight);
}

#endif // AutoH3D_h_
