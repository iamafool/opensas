#ifndef UTILITY_H
#define UTILITY_H

#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <sstream>

static std::string trim(const std::string& s) {
    if (s.empty()) return s;
    size_t start = 0;
    while (start < s.size() && (s[start] == ' ' || s[start] == '\t' ||
        s[start] == '\r' || s[start] == '\n')) {
        start++;
    }
    if (start == s.size()) return ""; // All whitespace
    size_t end = s.size() - 1;
    while (end > start && (s[end] == ' ' || s[end] == '\t' ||
        s[end] == '\r' || s[end] == '\n')) {
        end--;
    }
    return s.substr(start, end - start + 1);
}

static std::string getLibname(const std::string& dsName)
{
    std::string lib("WORK");
    size_t dotPos = dsName.find('.');
    if (dotPos != std::string::npos) {
        lib = dsName.substr(0, dotPos);
    }
    return lib;
}


#endif // !UTILITY_H

