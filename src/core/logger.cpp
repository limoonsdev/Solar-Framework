#include "solar/core/logger.hpp"
#include <cstdio>
#include <cstdarg>
#include <ctime>

namespace Solar {

    Logger& Logger::Get() {
        static Logger instance;
        return instance;
    }

    void Logger::Log(LogLevel level, const char* fmt, ...) {
        if (static_cast<int>(level) < static_cast<int>(m_minLevel)) return;

        const char* prefix = "[INFO]";
        switch (level) {
        case LogLevel::Trace: prefix = "[TRACE]"; break;
        case LogLevel::Debug: prefix = "[DEBUG]"; break;
        case LogLevel::Info:  prefix = "[INFO] "; break;
        case LogLevel::Warn:  prefix = "[WARN] "; break;
        case LogLevel::Error: prefix = "[ERROR]"; break;
        case LogLevel::Fatal: prefix = "[FATAL]"; break;
        }

        char buffer[2048];
        va_list args;
        va_start(args, fmt);
        vsnprintf(buffer, sizeof(buffer), fmt, args);
        va_end(args);

        time_t rawtime;
        time(&rawtime);
        struct tm* ti = localtime(&rawtime);
        char timeStr[16];
        strftime(timeStr, sizeof(timeStr), "%H:%M:%S", ti);

        printf("[%s] %s %s\n", timeStr, prefix, buffer);
    }

} // namespace Solar
