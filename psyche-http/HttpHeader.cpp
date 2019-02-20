#include "HttpHeader.h"

void HttpHeader::set(const std::string& key, const std::string& value) {
    if (value.length() == 0) erase(key);
    else header_[key] = value;
}

void HttpHeader::erase(const std::string& key) {
    header_.erase(key);
}

HttpHeader::Header HttpHeader::parseHeader(const std::string& header) {
    std::map<std::string, std::string> result;
    auto it = header.begin();
    auto last_it = it;
    int i = 0;
    for (;; ++it) {
        if (*it == ' ' || *it == '\r') {
            if (i == 0) {
                result["method"] = std::string(last_it, it);
            }
            else if (i == 1) {
                result["path"] = std::string(last_it, it);
            }
            else {
                result["protocol"] = std::string(last_it, it);
                last_it = it + 2;
                break;
            }
            ++i;
            last_it = it + 1;
        }
    }
    std::string cur_key, cur_val;
    bool flag = true;
    for (; it != header.end(); ++it) {
        if (flag) {
            if (*it == ':') {
                cur_key = std::string(last_it, it);
                if (*(it + 1) == ' ') ++it;
                last_it = it + 1;
                flag = !flag;
            }
        }
        else {
            if (*it == '\r') {
                cur_val = std::string(last_it, it);
                if (cur_val == "\r\n") break;
                result[cur_key] = cur_val;
                it += 2;
                last_it = it;
                flag = !flag;
            }
        }
    }
    return result;
}

std::string HttpHeader::get(const std::basic_string<char>& key) {
    auto it = header_.find(key);
    if (it != header_.end()) return it->second;
    return std::string();
}


std::string HttpHeader::to_string() {
    std::string s;
    s += "HTTP/1.1 ";
    switch (status_code_) {
    case 200:
        s += "200 OK";
        break;
    case 404:
        s += "404 NOT FOUND";
        break;
    case 400:
        s += "400 BAD REQUEST";
        break;
    default:
        s += "400 BAD REQUEST";
        break;
    }
    s += "\r\n";
    for (auto it = header_.begin(); it != header_.end(); ++it) {
        s += it->first + ": " + it->second + "\r\n";
    }
    s += "\r\n";
    return s;
}