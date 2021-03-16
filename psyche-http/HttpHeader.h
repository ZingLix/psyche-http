#pragma once
#include <map>
#include <string>

namespace psyche_http
{
class HttpHeader
{
public:
    enum class Method
    {
        NUL,
        GET,
        HEAD,
        PUT,
        POST,
        TRACE,
        OPTIONS,
        DELETE,
        PATCH,
        CONNECT
    };

    HttpHeader();
    HttpHeader(const std::string& s);
    HttpHeader(const HttpHeader& other);
    HttpHeader(HttpHeader&& other) noexcept;

    HttpHeader& operator=(const HttpHeader& other);
    HttpHeader& operator=(HttpHeader&& other) noexcept;

    virtual ~HttpHeader();

    void set(const std::string& key, const std::string& value);
    std::string get(const std::string& key);
    void erase(const std::string& key);

    void set_header(const std::string& msg);
    std::string to_string();

protected:
    using Header = std::map<std::string, std::string>;

    static Method string_to_method(const std::string& s);
    virtual size_t parse_first_line(const std::string& msg) = 0;
    Header parse_header(const std::string& header);
    virtual std::string first_line() = 0;

    Header header_;
};

class HttpRequestHeader : public HttpHeader
{
public:
    HttpRequestHeader();
    HttpRequestHeader(const std::string& message);
    HttpRequestHeader(const HttpRequestHeader& other);
    HttpRequestHeader(HttpRequestHeader&& other) noexcept;

    HttpRequestHeader& operator=(const HttpRequestHeader& other);
    HttpRequestHeader& operator=(HttpRequestHeader&& other) noexcept;

    [[nodiscard]]
    std::string get_path() const;
    [[nodiscard]]
    Method get_method() const;
    [[nodiscard]]
    std::string get_protocol() const;

protected:
    size_t parse_first_line(const std::string& msg) override;
    std::string first_line() override;

    Method method_;
    std::string path_;
    std::string protocol_;
};

class HttpResponseHeader : public HttpHeader
{
public:
    HttpResponseHeader();

    void set_protocol(const std::string& protocol);
    void set_status_code(int status_code);

protected:
    std::string first_line() override;
    static const char* status_code_definition(int status_code);
    size_t parse_first_line(const std::string& msg) override;

    std::string protocol_;
    int status_code_;
};
}
