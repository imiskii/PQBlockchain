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
#include "PQBconstants.hpp"

namespace PQB{

    std::shared_ptr<spdlog::logger> Log::s_Logger;

    void Log::init(){
        s_Logger = spdlog::basic_logger_mt("PQB", LOG_FILE.data());
        s_Logger->flush_on(spdlog::level::trace);
        s_Logger->set_level(spdlog::level::trace);
        s_Logger->set_pattern("%^[%l] %v%$");
    }


    std::string_view shortStr(const std::string_view &input, size_t maxLength){
        if (input.size() <= maxLength){
            return input;
        } else {
            return input.substr(0, maxLength);
        }
        return std::string_view();
    }

} // namespace PQB

/* END OF FILE */