#include "Utility.hpp"

std::vector<std::string> ft_Split(const std::string &str, const char &delimiter)
{
    std::vector<std::string> ret;
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delimiter))
    {
        ret.push_back(token);
    }
    return ret;
}