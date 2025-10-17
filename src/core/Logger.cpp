#include "Logger.h"

void Logger::log(const std::string& message) {
    auto now = std::chrono::system_clock::now();
    auto timePoint = std::chrono::system_clock::to_time_t(now);
    auto localTime = *localtime(&timePoint);

    std::stringstream ss;
    ss << "[" << std::put_time(&localTime, "%H:%M:%S") << "] " << message << "\n";
    appState.logMessages += ss.str();

    trimLogIfNeeded();
}

void Logger::clear() {
    appState.logMessages.clear();
}

void Logger::trimLogIfNeeded() {
    size_t lineCount = std::count(appState.logMessages.begin(),
                             appState.logMessages.end(), '\n');

    if (lineCount > Config::MAX_LOG_LINES) {
        size_t firstNewline = appState.logMessages.find('\n');
        if (firstNewline != std::string::npos) {
            appState.logMessages = appState.logMessages.substr(firstNewline + 1);
        }
    }
}