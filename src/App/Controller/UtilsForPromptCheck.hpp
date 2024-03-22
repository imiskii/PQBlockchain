/**
 * @file UtilsForPromptCheck.hpp
 * @author Michal Ľaš
 * @brief Functions that are used to check imput from a console
 * @date 2024-03-22
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#pragma once

#include <string>
#include <cctype>

namespace PQB{

/// @brief Check if given string is a number 
bool isNumber(const std::string& str);

/// @brief Check if given string contains only hexadecimal characters
bool isHexadecimal(const std::string& str);


} // namespace PQB

/* END OF FILE */