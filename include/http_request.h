#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include <string>

bool sendHttpRequest(const std::string &url, const std::string &authorization, std::string &response, const std::string &requestType);
bool checkLoggedIn(const std::string &port, const std::string &authorization);
bool getSummonerData(const std::string& port, const std::string& authorization, std::string& summonerData);

#endif // HTTP_REQUEST_H
