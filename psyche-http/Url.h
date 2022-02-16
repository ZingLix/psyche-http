#pragma once
#include <string>
#include <utility>
#include <vector>
#include <unordered_map>
#include <stdexcept>

namespace psyche_http
{
    struct Url
    {
        Url(std::string url);
        ~Url() {}

        std::vector<std::string> path_list;
        std::unordered_map<std::string, std::string> parameters;
    };

}