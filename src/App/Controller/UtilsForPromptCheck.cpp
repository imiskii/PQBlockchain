/**
 * @file UtilsForPromptCheck.cpp
 * @author Michal Ľaš
 * @brief Functions that are used to check imput from a console
 * @date 2024-03-22
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "UtilsForPromptCheck.hpp"


namespace PQB{

bool isNumber(const std::string &str) {
    try {
        size_t pos = 0;
        std::stoul(str, &pos);
        return pos == str.size();
    } catch (...) {
        return false;
    }
}

bool isHexadecimal(const std::string &str) {
    for (char c : str) {
        if (!std::isxdigit(c)) {
            return false;
        }
    }
    return true;
}


} // namespace PQB

/* END OF FILE */