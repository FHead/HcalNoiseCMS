#include <sstream>
#include <iostream>
#include <stdexcept>
#include <algorithm>

#include "HistogramManager.h"

HistogramManager::HistogramManager(const std::string& outputfile,
                                   const std::set<std::string>& histoTags)
    : outputfile_(outputfile.c_str(), "RECREATE")
{
    if (!outputfile_.IsOpen())
    {
        std::ostringstream os;
        os << "In HistogramManager constructor: failed to open file \""
           << outputfile << '"';
        throw std::invalid_argument(os.str());
    }
    const std::set<std::string>::const_iterator end = histoTags.end();
    for (std::set<std::string>::const_iterator it = histoTags.begin();
         it != end; ++it)
    {
        // Check if the tag contains any of the regex special characters
        if (it->find_first_of("^$\\.*+?()[]{}|") != std::string::npos)
            requestedRegex_.push_back(std::regex(*it));
        else
            requestedHistos_.insert(*it);
    }
}

bool HistogramManager::isRequested(const std::string& tag)
{
    // First, check for a direct match among non-regex expressions
    if (requestedHistos_.find(tag) != requestedHistos_.end())
    {
        checkedHistos_.insert(tag);
        return true;
    }

    // Now, cycle over regex expressions and see if any one of them matches
    const unsigned nReg = requestedRegex_.size();
    for (unsigned i=0; i<nReg; ++i)
        if (std::regex_match(tag.begin(), tag.end(), requestedRegex_[i]))
            return true;

    return false;
}

bool HistogramManager::tagMatches(const std::string& tag) const
{
    // First, check for a direct match among non-regex expressions
    if (requestedHistos_.find(tag) != requestedHistos_.end())
        return true;

    // Now, cycle over regex expressions and see if any one of them matches
    const unsigned nReg = requestedRegex_.size();
    for (unsigned i=0; i<nReg; ++i)
        if (std::regex_match(tag.begin(), tag.end(), requestedRegex_[i]))
            return true;

    return false;
}

std::set<std::string> HistogramManager::requestsNotChecked() const
{
    std::set<std::string> result;
    std::set_difference(requestedHistos_.begin(), requestedHistos_.end(),
                        checkedHistos_.begin(), checkedHistos_.end(),
                        std::inserter(result, result.end()));
    return result;
}

bool HistogramManager::verifyHistoRequests() const
{
    const std::set<std::string>& result = requestsNotChecked();
    if (result.empty())
        return true;
    else
    {
        std::cerr << "Problem in HistogramManager::verifyHistoRequests: "
                  << "the following item request"
                  << (result.size() > 1 ? "s" : "")
                  << " could not be processed: \"";
        bool notfirst = false;
        const std::set<std::string>::const_iterator end = result.end();
        for (std::set<std::string>::const_iterator it = result.begin();
             it != end; ++it)
        {
            if (notfirst)
                std::cerr << "\", \"";
            std::cerr << *it;
            notfirst = true;
        }
        std::cerr << '"' << std::endl;
        return false;
    }
}

TDirectory* HistogramManager::findOrMakeDirectory(const std::string& dirname)
{
    TDirectory* dir = &outputfile_;
    if (!dirname.empty())
    {
        std::istringstream is(dirname);
        std::string token;
        while (std::getline(is, token, '/'))
            if (!token.empty())
            {
                const char* path = token.c_str();
                TDirectory* search = dir->GetDirectory(path);
                if (search == 0)
                    search = new TDirectoryFile(path, path, "", dir);
                dir = search;
            }
    }
    return dir;
}

void HistogramManager::manage(ManagedHisto* h, const char* group)
{
    assert(h);
    h->SetDirectory(findOrMakeDirectory(h->GetDirectoryName()));
    if (group)
        groups_[group].push_back(h);
    else
        histos_.push_back(h);
}

void HistogramManager::CycleFill(const unsigned nCycles, const char* group,
                                 const bool throwException)
{
    ManagedHistoContainer* hvec = &histos_;
    if (group)
    {
        Groups::iterator it = groups_.find(group);
        if (it != groups_.end())
            hvec = &it->second;
        else
        {
            if (throwException)
                throw std::runtime_error("HistogramManager::CycleFill "
                                         "called for an invalid group");
            else
                return;
        }
    }
    hvec->CycleFill(nCycles);
}

void HistogramManager::AutoFill(const char* group, const bool throwException)
{
    ManagedHistoContainer* hvec = &histos_;
    if (group)
    {
        Groups::iterator it = groups_.find(group);
        if (it != groups_.end())
            hvec = &it->second;
        else
        {
            if (throwException)
                throw std::runtime_error("HistogramManager::AutoFill "
                                         "called for an invalid group");
            else
                return;
        }
    }
    hvec->AutoFill();
}

TObject* HistogramManager::FindByName(const char* name,
                                      const char* group) const
{
    TObject* obj = 0;
    if (group)
    {
        Groups::const_iterator it = groups_.find(group);
        if (it != groups_.end())
            obj = it->second.FindByName(name);
    }
    else
        obj = histos_.FindByName(name);
    return obj;
}

TObject* HistogramManager::LastRootItem(const char* group) const
{
    TObject* obj = 0;
    if (group)
    {
        Groups::const_iterator it = groups_.find(group);
        if (it != groups_.end())
            obj = it->second.LastRootItem();
    }
    else
        obj = histos_.LastRootItem();
    return obj;
}

std::size_t HistogramManager::NManaged(const char* group) const
{
    std::size_t sz = 0;
    if (group)
    {
        Groups::const_iterator it = groups_.find(group);
        if (it != groups_.end())
            sz = it->second.size();
    }
    else
        sz = histos_.size();
    return sz;
}

ManagedHisto* HistogramManager::GetManagedHisto(std::size_t index,
                                                const char* group) const
{
    if (group)
    {
        Groups::const_iterator it = groups_.find(group);
        if (it == groups_.end())
            return 0;
        else
            return it->second.at(index);
    }
    else
        return histos_.at(index);
}
