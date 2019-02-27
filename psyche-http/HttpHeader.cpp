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
    it += parseFirstLine(header);
    auto last_it = it;
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
    std::string s(firstLine());
    s += "\r\n";
    for (auto it = header_.begin(); it != header_.end(); ++it) {
        s += it->first + ": " + it->second + "\r\n";
    }
    s += "\r\n";
    return s;
}