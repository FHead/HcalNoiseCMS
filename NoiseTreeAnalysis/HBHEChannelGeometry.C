#include <cassert>
#include <sstream>
#include <fstream>
#include <stdexcept>

#include "HBHEChannelGeometry.h"
#include "HBHEChannelMap.h"

#include "npstat/stat/InMemoryNtuple.hh"

using namespace npstat;

HBHEChannelGeometry::HBHEChannelGeometry(const char* hbFile, const char* heFile)
    : directions_(HBHEChannelMap::ChannelCount)
{
    HBHEChannelMap chmap;

    // Load the data from text files
    loadData(hbFile, chmap);
    loadData(heFile, chmap);

    // Check that we have directions for all channels
    TVector3 zero;
    for (unsigned i=0; i<HBHEChannelMap::ChannelCount; ++i)
        if (directions_[i] == zero)
        {
            unsigned depth, iphi;
            int ieta;
            chmap.getChannelTriple(i, &depth, &ieta, &iphi);
            std::ostringstream os;
            os << "In HBHEChannelGeometry constructor: no data for ieta "
               << ieta << ", iphi " << iphi << ", depth " << depth;
            throw std::runtime_error(os.str());
        }
}

void HBHEChannelGeometry::loadData(const char* filename,
                                   const HBHEChannelMap& chmap)
{
    std::ifstream is(filename);
    if (!is.is_open())
    {
        std::ostringstream os;
        os << "In HBHEChannelGeometry::loadData: failed to open file \""
           << filename << '"';
        throw std::invalid_argument(os.str());
    }

    InMemoryNtuple<double> nt(ntupleColumns("ieta","iphi","depth","x","y","z"));
    if (!fillNtupleFromText(is, &nt))
    {
        std::ostringstream os;
        os << "In HBHEChannelGeometry::loadData: failed to parse file \""
           << filename << '"';
        throw std::invalid_argument(os.str());
    }

    double buf[6];
    const unsigned nrows = nt.nRows();
    for (unsigned row=0; row<nrows; ++row)
    {
        nt.rowContents(row, buf, sizeof(buf)/sizeof(buf[0]));
        const unsigned idx = chmap.linearIndex(buf[2], buf[0], buf[1]);
        TVector3 vec(&buf[3]);
        directions_.at(idx) = vec.Unit();
    }
}
