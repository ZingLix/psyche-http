#pragma once
#include "HttpHeader.h"
#include <string>
#include <memory>

namespace psyche_http
{
    class HttpMessage
    {
    public:
        HttpMessage();
        HttpMessage(const HttpMessage& other);
        HttpMessage(HttpMessage&& other) noexcept;

        HttpMessage& operator=(const HttpMessage& other);
        HttpMessage& operator=(HttpMessage&& other) noexcept;

        virtual ~HttpMessage();

        void append_body(const std::string& str);
        [[nodiscard]]
        size_t body_length() const;

        virtual std::string to_string() = 0;

    protected:
        std::string body_;
    };

    class HttpRequest : public HttpMessage
    {
    public:
        HttpRequest();

        HttpRequest(const std::string& message);

        std::string get(const std::string& key);
        [[nodiscard]]
        std::string get_path() const;
        [[nodiscard]]
        std::string get_protocol() const;
        [[nodiscard]]
        HttpHeader::Method get_method() const;

        std::string to_string() override;
        std::string get_arguments_value(const std::string& argument);
        void set_arguments(std::map<std::string, std::string>&& arguments);
        void parseHeader(const std::string& msg);

        ~HttpRequest() override;

    private:
        HttpRequestHeader header_;
        std::map<std::string, std::string> arguments_;
    };

    class HttpResponse : public HttpMessage
    {
    public:
        HttpResponse();

        void set_status_code(int status_code);
        void set_content_length();
        void set_protocol(std::string protocol);
        void set(const std::string& key, const std::string& value);

        std::string to_string() override;

        ~HttpResponse() override;

    private:
        HttpResponseHeader header_;
    };
}
