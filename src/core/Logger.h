#pragma once
#include <string>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include "../state/ApplicationState.h"

class Logger {
public:
    static void log(const std::string& message);
    static void clear();

private:
    static void trimLogIfNeeded();
};
