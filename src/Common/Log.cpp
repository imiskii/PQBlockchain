/**
 * @file Log.cpp
 * @author Michal Ľaš
 * @brief Logger for PQB
 * @date 2024-03-21
 * 
 * @copyright Copyright (c) 2024
 * 
 */


#include "Log.hpp"
#include "PQBconstatnts.hpp"

namespace PQB{

    std::shared_ptr<spdlog::logger> Log::s_Logger;

    void Log::init(){
        s_Logger = spdlog::basic_logger_mt("PQB", LOG_FILE);
        s_Logger->flush_on(spdlog::level::trace);
        s_Logger->set_level(spdlog::level::trace);
        s_Logger->set_pattern("%^[%l] %v%$");
    }

} // namespace PQB

/* END OF FILE */