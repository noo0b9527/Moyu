#pragma once

#include <format>
#include <source_location>
#include <string_view>


#include "Base/Config.h"

namespace Moyu
{
    enum class LogLevelType
    {
        Unkown = 0,
        Debug,
        Information,
        Warning,
        Error,
        Fatal,
        Max,
    };

    class Logger
    {
    public:
        explicit Logger(const std::source_location location =
                            std::source_location::current());

        ~Logger() = default;

        template<typename... Args>
        inline void Debug(const std::format_string<Args...> fmt, Args&&... args)
        {
            auto message =
                std::vformat(fmt.get(), std::make_format_args(args...));
            Log(LogLevelType::Debug, message, m_location);
        }

        template<typename... Args>
        inline void Info(const std::format_string<Args...> fmt, Args&&... args)
        {
            auto message =
                std::vformat(fmt.get(), std::make_format_args(args...));
            Log(LogLevelType::Information, message, m_location);
        }

        template<typename... Args>
        inline void Warning(const std::format_string<Args...> fmt,
                            Args&&... args)
        {
            auto message =
                std::vformat(fmt.get(), std::make_format_args(args...));
            Log(LogLevelType::Warning, message, m_location);
        }

        template<typename... Args>
        inline void Error(const std::format_string<Args...> fmt, Args&&... args)
        {
            auto message =
                std::vformat(fmt.get(), std::make_format_args(args...));
            Log(LogLevelType::Error, message, m_location);
        }

        template<typename... Args>
        inline void Fatal(const std::format_string<Args...> fmt, Args&&... args)
        {
            auto message =
                std::vformat(fmt.get(), std::make_format_args(args...));
            Log(LogLevelType::Fatal, message, m_location);
        }

    private:
        void Log(LogLevelType level, std::string_view message,
                 std::source_location& loc);

        std::source_location m_location;
    };

    Logger
    Log(const std::source_location location = std::source_location::current());

} // namespace Moyu
