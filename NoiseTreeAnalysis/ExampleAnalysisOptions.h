#ifndef ExampleAnalysisOptions_h_
#define ExampleAnalysisOptions_h_

#include <iostream>

#include "CmdLine.hh"

//
// Class ExampleAnalysisOptions must have
//
// 1) Default constructor
//
// 2) Copy constructor (usually auto-generated)
//
// 3) Method "void parse(CmdLine& cmdline)"
//
// 4) Method "void usage(std::ostream& os) const" for printing usage
//    instructions
//
// Preferably, this class should also have "operator<<" for printing
// the option values actually used.
//
// This class works in tandem with the analysis class.
// ExampleAnalysisOptions object is a "const" member in the analysis
// class, so it is safe to make ExampleAnalysisOptions a struct.
//
// The "parse" method must use normal methods of "CmdLine"
// ("option", "has", and "require") to fill the members of
// this class. Note that, if you find yourself using method
// "option" to assign values to some members, you should
// initialize these members in the default constructor.
//
// Do not use here switches reserved for use by the main program.
// These switches are:
//   "-h", "--histogram"
//   "-n", "--maxEvents"
//   "-s", "--noStats"
//   "-t", "--treeName"
//   "-v", "--verbose"
//
struct ExampleAnalysisOptions
{
    ExampleAnalysisOptions()
        : exampleOption(0)
    {
    }

    void parse(CmdLine& cmdline)
    {
        cmdline.option(NULL, "--exampleOption") >> exampleOption;
    }

    void usage(std::ostream& os) const
    {
        os << "[--exampleOption value]";
    }

    int exampleOption;
};

std::ostream& operator<<(std::ostream& os, const ExampleAnalysisOptions& o)
{
    os << "exampleOption = " << o.exampleOption;
    return os;
}

#endif // ExampleAnalysisOptions_h_
