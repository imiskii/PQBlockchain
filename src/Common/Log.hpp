/**
 * @file Log.hpp
 * @author Michal Ľaš
 * @brief Logger for PQB
 * @date 2024-03-21
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#pragma once

#include <memory>
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"


namespace PQB{


class Log{
public:

    static void init();

    inline static std::shared_ptr<spdlog::logger> &GetLogger() { return s_Logger; }

private:

    static std::shared_ptr<spdlog::logger> s_Logger;

};

} // namespace PQB

#define PQB_LOG_TRACE(module, ...)  ::PQB::Log::GetLogger()->trace("{}: {}", module, fmt::format(__VA_ARGS__))
#define PQB_LOG_INFO(module, ...)   ::PQB::Log::GetLogger()->info("{}: {}", module, fmt::format(__VA_ARGS__))
#define PQB_LOG_WARN(module, ...)   ::PQB::Log::GetLogger()->warn("{}: {}", module, fmt::format(__VA_ARGS__))
#define PQB_LOG_ERROR(module, ...)  ::PQB::Log::GetLogger()->error("{}: {}", module, fmt::format(__VA_ARGS__))

/* END OF FILE */