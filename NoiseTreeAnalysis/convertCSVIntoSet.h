#ifndef convertCSVIntoSet_h_
#define convertCSVIntoSet_h_

//
// A simple facility for converting a string of comma-separated values
// into a set of strings
//
// I. Volobouev
// March 2013
//

#include <set>
#include <string>
#include <sstream>

inline std::set<std::string> convertCSVIntoSet(const std::string& input)
{
    const char* white = " \f\n\r\t\v";
    std::set<std::string> result;
    if (!input.empty())
    {
        std::istringstream is(input);
        std::string token;
        while (std::getline(is, token, ','))
        {
            // Discard leading and trailing white space, if any
            const std::size_t first = input.find_first_not_of(white);
            if (first != std::string::npos)
            {
                const std::size_t last = input.find_last_not_of(white);
                result.insert(std::string(token, first, last+1U-first));
            }
        }
    }
    return result;
}

#endif // convertCSVIntoSet_h_
