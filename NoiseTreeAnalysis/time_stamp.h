#ifndef TIME_STAMP_H_
#define TIME_STAMP_H_

//
// Return the string representing current time in the format hh:mm:ss
//
// I. Volobouev
// March 2013
//

#include <ctime>
#include <cstdio>
#include <string>

inline std::string time_stamp()
{
    struct tm *current;
    time_t now;

    time(&now);
    current = localtime(&now);

    char buf[10];
    sprintf(buf, "%02i:%02i:%02i", current->tm_hour,
            current->tm_min, current->tm_sec);
    return std::string(buf);
}

#endif // TIME_STAMP_H_
