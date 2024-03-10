#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include <string>

bool sendHttpRequest(const std::string &url, const std::string &authorization, std::string &response, const std::string &requestType, int retryDelaySeconds = 1);
bool checkLoggedIn(const std::string &port, const std::string &authorization, int retryDelaySeconds = 1);
bool getSummonerData(const std::string& port, const std::string& authorization, std::string& summonerData, int retryDelaySeconds = 1);

#endif // HTTP_REQUEST_H
