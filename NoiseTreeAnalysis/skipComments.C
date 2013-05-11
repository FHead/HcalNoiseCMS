#include <cctype>
#include <cassert>
#include <fstream>

#include "skipComments.h"

using namespace std;

bool skipComments(const char* filename, std::vector<std::string>* lines)
{
    assert(lines);
    lines->clear();

    ifstream is(filename);
    if (!is.is_open())
        return false;

    std::string line;
    while (is)
    {
        std::getline(is, line);
        const unsigned long len = line.size();
        if (len == 0UL)
            continue;

        // Find the first non-space character
        unsigned long firstNSpace = 0;
        for (; firstNSpace<len && isspace(line[firstNSpace]); ++firstNSpace) {;}

        // Skip if this is a white space line
        if (firstNSpace == len)
            continue;

        // Skip if the first character after white space is '#' (comment)
        if (line[firstNSpace] == '#')
            continue;

        // Find the character beyond the last non-space character
        unsigned long lastNSpace = len;
        for (; lastNSpace && isspace(line[lastNSpace-1UL]); --lastNSpace) {;}

        // Push the non-white part of the string into output
        if (firstNSpace == 0 && lastNSpace == len)
            lines->push_back(line);
        else
        {
            assert(firstNSpace < lastNSpace);
            lines->push_back(line.substr(firstNSpace, lastNSpace-firstNSpace));
        }
    }

    return !is.bad() && is.eof();
}
