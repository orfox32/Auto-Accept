#include "http_request.h"
#include <iostream>
#include <curl/curl.h>
#include <string>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>

using namespace std;

size_t WriteCallback(void *contents, size_t size, size_t nmemb, string *buffer)
{
    size_t totalSize = size * nmemb;
    buffer->append((char *)contents, totalSize);
    return totalSize;
}

bool sendHttpRequest(const string &url, const string &authorization, string &response, const string &requestType)
{
    CURL *curl;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    if (!curl)
    {
        cerr << "Failed to initialize CURL." << endl;
        return false;
    }

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    if (requestType == "POST")
    {
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
    }
    else
    {
        curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
    }
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Accept: application/json");
    headers = curl_slist_append(headers, authorization.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    if (requestType == "POST")
    {
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "");
    }

    res = curl_easy_perform(curl);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK)
    {
        cerr << "Error: " << curl_easy_strerror(res) << endl;
        return false;
    }

    return true;
}

bool checkLoggedIn(const string &port, const string &authorization)
{
    string url = "https://127.0.0.1:" + port + "/lol-login/v1/session";
    string response;

    return sendHttpRequest(url, authorization, response, "GET");
}