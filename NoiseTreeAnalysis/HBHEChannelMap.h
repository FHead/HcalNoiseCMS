#ifndef HBHEChannelMap_h_
#define HBHEChannelMap_h_

//
// Numerology for finding collections of HCAL channels that belong
// to the same HPD and/or navigating collections of neighbors.
// 
// Various tables are made in the class constructor and stored internally
// for future lookup. Therefore, this class will be used most efficiently
// if one instance of it is created at the beginning of your program and
// then reused as needed.
//
// I. Volobouev
// March 2013
//

#include <map>
#include <vector>

#include "npstat/nm/Triple.hh"

#include "HcalSubdetector.h"
#include "HcalHPDRBXMap.h"

class HBHEChannelMap
{
public:
    enum {ChannelCount = 5184U};

    HBHEChannelMap();

    // Mapping from the depth/ieta/iphi triple which uniquely
    // identifies an HBHE channels into a linear index, currently
    // from 0 to 5183 (inclusive). This linear index should not
    // be treated as anything meaningful -- consider it to be
    // just a convenient unique key in a database table.
    unsigned linearIndex(unsigned depth, int ieta, unsigned iphi) const;

    // Check whether the given triple is a valid depth/ieta/iphi combination
    bool isValidTriple(unsigned depth, int ieta, unsigned iphi) const;

    // Inverse mapping, from a linear index into depth/ieta/iphi triple.
    // Any of the argument pointers is allowed to be NULL in which case
    // the corresponding variable is simply not filled out.
    void getChannelTriple(unsigned index, unsigned* depth,
                          int* ieta, unsigned* iphi) const;

    // Lookup the HPD numbers from the linear channel number. The HPD
    // numbering convention comes from the HcalHPDRBXMap class (not
    // terribly meaningful either).
    unsigned getHPD(unsigned channelNumber) const;
    unsigned getChannelInHPD(unsigned channelNumber) const;

    // Lookup the RBX numbers from the linear channel number
    unsigned getRBX(unsigned channelNumber) const;
    unsigned getChannelInRBX(unsigned channelNumber) const;

    // Lookup the list of channels geometrically neighboring the given
    // channel but coming from other HPDs
    const std::vector<unsigned>& channelNeigborsFromOtherHPDs(
        unsigned channelNumber) const;

    // Fill unique neighbors for the given set of channels. This method
    // assumes that all input channels come from a single HPD.
    void channelSetNeighbors(const std::vector<unsigned>& input,
                             std::vector<unsigned>* output) const;

    // Look up linear channel indices for a given HPD
    inline const std::vector<unsigned>& getHPDChannels(unsigned hpd) const
        {return hpd_channel_lookup_.at(hpd);}

    // Look up linear channel indices for all neighbors of a given HPD
    const std::vector<unsigned>& getHPDNeigbors(unsigned hpd) const;

    // Look up linear channel indices for a given RBX
    inline const std::vector<unsigned>& getRBXChannels(unsigned rbx) const
        {return rbx_channel_lookup_.at(rbx);}

    // Maximum number of channels per HPD
    unsigned maxChannelsPerHPD() const;

    // Maximum number of channels per RBX
    unsigned maxChannelsPerRBX() const;

    // The following assumes a valid HBHE depth/ieta combination
    static HcalSubdetector getSubdetector(unsigned depth, int ieta);

private:
    //                      depth, ieta, iphi
    typedef npstat::Triple<unsigned,int,unsigned> ChannelId;
    typedef std::map<ChannelId,unsigned> ChannelMap;

    ChannelId lookup_[ChannelCount];
    ChannelMap inverse_;

    unsigned hpd_lookup_[ChannelCount];
    unsigned chan_in_hpd_lookup_[ChannelCount];
    std::vector<std::vector<unsigned> > hpd_channel_lookup_;

    unsigned rbx_lookup_[ChannelCount];
    unsigned chan_in_rbx_lookup_[ChannelCount];
    std::vector<std::vector<unsigned> > rbx_channel_lookup_;

    std::vector<unsigned> channel_neighbors_[ChannelCount];
    bool neighborInfoFilled_;

    std::vector<unsigned> hpd_neighbors_[HcalHPDRBXMap::NUM_HPDS];
    bool hpdNeighborsFilled_;

    void calculateNeighborList(unsigned index, std::vector<unsigned>*) const;
    void calculateHPDNeighbors(unsigned hpd, std::vector<unsigned>*) const;
};

#endif // HBHEChannelMap_h_
