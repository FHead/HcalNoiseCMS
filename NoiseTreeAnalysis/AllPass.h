#ifndef AllPass_h_
#define AllPass_h_

//
// The "all-pass" filter for use with automatic ntuples
//
// I. Volobouev
// March 2013
//

struct AllPass
{
    inline bool operator()() const {return true;}
    inline bool operator()(unsigned) const {return true;}
};

#endif // AllPass_h_
