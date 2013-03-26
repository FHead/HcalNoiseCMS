#ifndef Column_h_
#define Column_h_

//
// Column definition helper function for use with AutoNtuple, AutoNtupleD,
// CycledNtuple, and CycledNtupleD.
//
// I. Volobouev
// March 2013
//

//
// In the user code, do not create instances of ColumnHelper class
// directly, use the "Column" function instead
//
template<class Functor>
struct ColumnHelper
{
    inline ColumnHelper(const std::string& iName, Functor ifcn)
        : name(iName), fcn(ifcn) {}

    std::string name;
    Functor fcn;
};

template<class Functor>
inline ColumnHelper<Functor> Column(const std::string& iName, Functor ifcn)
{
    return ColumnHelper<Functor>(iName, ifcn);
}

//
// Convenience macro for variables that either already exist in a root tree
// we are processing or belong to the analysis class
//
#define TreeDatum(name) Column( #name , ValueOf(this->name))

#endif // Column_h_
