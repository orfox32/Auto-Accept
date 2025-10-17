#pragma once
#include <string>
#include <curl/curl.h>
#include "../core/Logger.h"

class HttpClient {
public:
    static bool sendRequest(const std::string& url,
                          const std::string& authHeader,
                          std::string& response,
                          const std::string& method = "GET");

    static std::string buildUrl(const std::string& port, const std::string& endpoint);

private:
    static size_t writeCallback(void* contents, size_t size, size_t nmemb, std::string* buffer);
};
