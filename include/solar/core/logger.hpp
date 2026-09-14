#pragma once
#include "types.hpp"
#include <string>

namespace Solar {

    enum class LogLevel {
        Trace = 0,
        Debug,
        Info,
        Warn,
        Error,
        Fatal
    };

    class Logger {
    public:
        static Logger& Get();
        void Log(LogLevel level, const char* fmt, ...);
        void SetMinLevel(LogLevel level) { m_minLevel = level; }

    private:
        Logger() = default;
        LogLevel m_minLevel = LogLevel::Info;
    };

} // namespace Solar
