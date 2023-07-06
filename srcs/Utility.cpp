#include "Utility.hpp"
#include <iostream>

std::vector<std::string> Split(const std::string &str, const char &delimiter)
{
    if (str.empty() || !str.compare(""))
        return (std::vector<std::string>());
    std::vector<std::string> ret;
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delimiter))
    {
        ret.push_back(token);
    }
    return ret;
}