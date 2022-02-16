#include "Url.h"

psyche_http::Url::Url(std::string url)
{
    size_t last = 0;
    if (url.size() == 0 || url[0] != '/') {
        throw std::invalid_argument("Bad Url");
    }
    for (size_t i = 1; i <= url.size(); ++i) {
        if (i == url.size() || url[i] == '/') {
            if (last + 1 == i - 1) {
                throw std::invalid_argument("Bad Url");
            }
            this->path_list.emplace_back(url.substr(last + 1, i - 1));
            last = i;
        }
    }
}