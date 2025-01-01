#ifndef UTILITY_H
#define UTILITY_H

#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <boost/flyweight.hpp>


// Define a flyweight string type
using flyweight_string = boost::flyweight<std::string>;

// Define a variant type that can hold either a flyweight string or a double
using Cell = std::variant<flyweight_string, double>;

using Value = std::variant<double, std::string>;

// In-place uppercase
static void to_upper_inplace(std::string& str) {
    std::transform(str.begin(), str.end(), str.begin(),
        [](unsigned char c) { return static_cast<unsigned char>(std::toupper(c)); }
    );
}

// In-place lowercase
static void to_lower_inplace(std::string& str) {
    std::transform(str.begin(), str.end(), str.begin(),
        [](unsigned char c) { return static_cast<unsigned char>(std::tolower(c)); }
    );
}

// Return-by-value uppercase
static std::string to_upper(std::string str) {
    to_upper_inplace(str);
    return str;
}

// Return-by-value lowercase
static std::string to_lower(std::string str) {
    to_lower_inplace(str);
    return str;
}

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

static Value cellToValue(const Cell& input)
{
    // Use std::visit with a lambda
    return std::visit(
        [](auto&& arg) -> Value {
            using T = std::decay_t<decltype(arg)>;
            // If we have a flyweight_string
            if constexpr (std::is_same_v<T, flyweight_string>) {
                // Convert to std::string via .get()
                return arg.get();
            }
            else {
                // must be double
                return arg;
            }
        },
        input
    );
}

static Cell valueToCell(const Value& input)
{
    // Use std::visit with a lambda
    return std::visit(
        [](auto&& arg) -> Cell {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                // Convert to std::string via .get()
                return flyweight_string(arg);
            }
            else {
                // must be double
                return arg;
            }
        },
        input
    );
}

#endif // !UTILITY_H

