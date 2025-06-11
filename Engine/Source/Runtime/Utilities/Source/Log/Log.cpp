#include "Utilities/Log.h"

#include "spdlog/spdlog.h"

namespace Moyu
{
    Logger::Logger(const std::source_location location) : m_location(location)
    {}

    void Logger::Log(LogLevelType level, std::string_view message,
                     std::source_location& loc)
    {
        switch (level)
        {
            case LogLevelType::Debug:
                spdlog::debug("[{}:{}:{}] {}", loc.file_name(), loc.line(),
                              loc.function_name(), message);
                break;
            case LogLevelType::Information:
                spdlog::info("[{}:{}:{}] {}", loc.file_name(), loc.line(),
                             loc.function_name(), message);
                break;
            case LogLevelType::Warning:
                spdlog::warn("[{}:{}:{}] {}", loc.file_name(), loc.line(),
                             loc.function_name(), message);
                break;
            case LogLevelType::Error:
                spdlog::error("[{}:{}:{}] {}", loc.file_name(), loc.line(),
                              loc.function_name(), message);
                break;
            case LogLevelType::Fatal:
                spdlog::critical("[{}:{}:{}] {}", loc.file_name(), loc.line(),
                                 loc.function_name(), message);
                break;
            default:
                spdlog::info("[{}:{}] {}", loc.function_name(), loc.line(),
                             message);
                break;
        }
    }

    Logger Log(const std::source_location location)
    {
        Logger current(location);
        return current;
    }

} // namespace Moyu
