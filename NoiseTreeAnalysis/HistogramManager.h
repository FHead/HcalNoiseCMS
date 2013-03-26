#ifndef HistogramManager_hh_
#define HistogramManager_hh_

//
// Manager class for histograms and ntuple wrappers which know how
// to fill underlying root objects. The intended sequence of steps
// in using this system is as follows:
//
// 1. Create an instance of HistogramManager.
//
// 2. Create histograms/ntuples using functions AutoH1D, AutoH2D,
//    AutoH3D, AutoNtuple, CycledH1D, CycledH2D, CycledH3D, and
//    CycledNtuple. All of these functions create an object of
//    their corresponding type on the heap and return a pointer
//    to that object.
//
//    It is often useful to create these histograms conditionally,
//    based on some command-line options. This class provides methods
//    "isRequested", "requestsNotChecked", and "verifyHistoRequests"
//    to assist in this mode of operation. See the class constructor
//    comments for more details.
//
// 3. Use the pointer to the object created in the previous step
//    as an argument to the "manage" method of the HistogramManager
//    instance. HistogramManager will assume the ownership of the
//    object and will call the object destructors from its own
//    desctructor. It is possible to group histograms and ntuples
//    into arbitrarily named groups -- just specify the group name
//    as the other argument of "manage".
//
// 4. At some point, after creation of all managed histograms,
//    call "verifyHistoRequests" (or "requestsNotChecked") and
//    check if all requested histograms were successfully created.
//
// 5. When the time comes to fill the histograms/ntuples, call the
//    "AutoFill" and/or "CycleFill" methods of the HistogramManager.
//    The manager will broadcast these calls to all members of the
//    corresponding object group. Note that, if you need to call
//    "CycleFill" with different number of cycles on different
//    histograms, these histograms should be managed in different
//    groups.
//
// I. Volobouev
// March 2013
//

#include <set>
#include <map>
#include <regex>

#include "ManagedHisto.h"
#include "TFile.h"

class HistogramManager
{
public:
    // We will create a new root file named "outputfile".
    //
    // "histoTags" is an arbitrary set of strings, presumably
    // specified on the command line. Internally, this set
    // will be split into two subsets: string which do contain
    // ECMAScript special characters "^$\.*+?()[]{}|" and strings
    // which don't. The strings which do contain special characters
    // will be passed to std::regex. The strings which do not
    // contain any special characters will form the other subset.
    //
    // Subsequently, every time the method "isRequested" is called,
    // its argument will be checked first against the subset of
    // normal strings. If a match is found, the argument will be
    // remembered as "processed" (and "true" is returned). Then
    // all regular expressions will be tested for a match and
    // "true" returned if at least one expression matches.
    //
    // After all necessary invocations of the "isRequested" method,
    // one can verify that all normal strings have been tested
    // for by "isRequested" checks. This can be done either by
    // "requestsNotChecked" method if the user wants to perform
    // his/her own processing of this information or by
    // "verifyHistoRequests" if the default processing (print
    // informative message about non-processed requests to std::cerr)
    // is sufficient.
    //
    HistogramManager(const std::string& outputfile,
                     const std::set<std::string>& histoTags);

    inline virtual ~HistogramManager()
        {if (outputfile_.IsOpen()) outputfile_.Write();}

    // If you want to create a root histo not managed by this manager
    // but still saved into the same file, call the "cd" method before
    // creating it.
    inline void cd()
        {outputfile_.cd();}

    inline void cd(const std::string& dirname)
        {findOrMakeDirectory(dirname)->cd();}

    // Check if the given tag is present in the set of "histoTags"
    // provided in the constructor. We will remember which checks
    // were made in order to compare them later with the original set.
    bool isRequested(const std::string& tag);

    // Just check if the argument matches one of the tags from "histoTags"
    // provided in the constructor. This works just like a const version
    // of "isRequested" but without remembering the checks.
    bool tagMatches(const std::string& tag) const;

    // Return requests that have not been checked by "isRequested"
    std::set<std::string> requestsNotChecked() const;

    // Return "true" if all requests have been checked by "isRequested".
    // Return "false" and print some info to std::cerr if some requests
    // have not been checked.
    bool verifyHistoRequests() const;

    // The "group" argument is an arbitrary name of the item group.
    // Grouping histograms and ntuples is useful if you need
    // to call "CycleFill" with different number of cycles for
    // different objects. Then you can group together all items
    // which require the same number of cycles. If "group" argument
    // is not provided, the default group will be used.
    void manage(ManagedHisto* h, const char* group=0);

    // By default, methods "AutoFill" and "CycleFill" will throw
    // an exception if they are called on a non-existent group
    // (the default group always exists even if it is empty).
    // If you want to simply ignore non-existent groups, set the
    // argument "throwExceptionIfGroupDoesNotExist" to false.
    virtual void AutoFill(const char* group=0,
                          bool throwExceptionIfGroupDoesNotExist=true);
    
    virtual void CycleFill(unsigned nCycles, const char* group=0,
                           bool throwExceptionIfGroupDoesNotExist=true);

    // Return the number of objects in the given group. 0 is returned
    // for non-existent groups.
    std::size_t NManaged(const char* group=0) const;

    // Return the item with the given number (in the order of "manage" calls)
    // from the given group
    ManagedHisto* GetManagedHisto(std::size_t index, const char* group=0) const;

    // Return the last item added to the given group. Will return NULL
    // in case the group does not exist (or empty).
    TObject* LastRootItem(const char* group=0) const;

    // Find an object in a group using its root name
    TObject* FindByName(const char* name, const char* group=0) const;

private:
    typedef std::map<std::string,ManagedHistoContainer> Groups;

    TDirectory* findOrMakeDirectory(const std::string& dirname);

    TFile outputfile_;
    std::set<std::string> requestedHistos_;
    std::set<std::string> checkedHistos_;
    std::vector<std::regex> requestedRegex_;
    ManagedHistoContainer histos_;
    Groups groups_;
};

#endif // HistogramManager_hh_
