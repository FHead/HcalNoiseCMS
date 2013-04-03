#ifndef HBHEChannelGeometry_h_
#define HBHEChannelGeometry_h_

//
// A simple geometry class for HCAL channels.
//
// Geometry is read from text files in the constructor. It is assumed
// that each line in such a file has 6 numbers: "ieta iphi depth x y z",
// where x, y, and z are the coordinates of the tower face center. The
// physical direction of the tower can be then looked up by channel number
// using the "getDirection" method.
//
// I. Volobouev
// April 2013
//

#include <vector>

#include "TVector3.h"

class HBHEChannelMap;

class HBHEChannelGeometry
{
public:
    HBHEChannelGeometry(const char* hbFile, const char* heFile);

    inline const TVector3& getDirection(const unsigned channel) const
         {return directions_.at(channel);}

private:
    void loadData(const char* filename, const HBHEChannelMap& chmap);

    std::vector<TVector3> directions_;
};

#endif // HBHEChannelGeometry_h_
