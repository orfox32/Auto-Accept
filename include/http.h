#pragma once
#include <string>

bool sendHttpRequest(const std::string& url, const std::string& authorization, std::string& response, const std::string& requestType);
bool checkLoggedIn(const std::string& port, const std::string& authorization);
std::string base64_encode(const std::string& input);
