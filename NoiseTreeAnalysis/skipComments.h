#ifndef SKIPCOMMENTS_H_
#define SKIPCOMMENTS_H_

#include <vector>
#include <string>

// The function returns "true" on success and "false"
// in case the file could not be open/read
bool skipComments(const char* filename, std::vector<std::string>* lines);

#endif // SKIPCOMMENTS_H_
