#include "HttpClient.h"

size_t HttpClient::writeCallback(void* contents, size_t size, size_t nmemb, std::string* buffer) {
    size_t totalSize = size * nmemb;
    buffer->append((char*)contents, totalSize);
    return totalSize;
}

bool HttpClient::sendRequest(const std::string& url,
                            const std::string& authHeader,
                            std::string& response,
                            const std::string& method) {
    CURL* curl;
    CURLcode result;

    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();

    if (!curl) {
        Logger::log("Failed to initialize CURL.");
        return false;
    }

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

    if (method == "POST") {
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "");
    } else {
        curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
    }

    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    struct curl_slist* headers = NULL;
    headers = curl_slist_append(headers, "Accept: application/json");
    headers = curl_slist_append(headers, authHeader.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    result = curl_easy_perform(curl);

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (result != CURLE_OK) {
        Logger::log("CURL Error: " + std::string(curl_easy_strerror(result)));
        return false;
    }

    return true;
}

std::string HttpClient::buildUrl(const std::string& port, const std::string& endpoint) {
    return "https://127.0.0.1:" + port + endpoint;
}