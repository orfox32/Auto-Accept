#ifndef UTILITY_H
#define UTILITY_H

#include <string>
#include <Windows.h>

bool extractAndEncodeRiotPassword(DWORD processId, std::string &port, std::string &encodedPassword);
bool authenticateAndAutoAccept(const std::string &port, const std::string &authorization);

#endif // UTILITY_H