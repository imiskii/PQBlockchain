/**
 * @file typedefs.hpp
 * @author Michal Ľaš
 * @brief Global typedefs for PQB
 * @date 2024-02-08
 * 
 * @copyright Copyright (c) 2024
 * 
 */


#pragma once

#include <cstdint>
#include <memory>
#include <vector>

namespace PQB{
    typedef unsigned char byte;     ///< 8-bit unsigned datatype
    typedef uint32_t cash;          ///< 4-byte datatype respresenting amount of currency
    typedef int64_t timestamp;     ///< 8-byte datatype representing a timestamp 
    typedef std::vector<PQB::byte> byteBuffer; ///< buffer with bytes
}



/* END OF FILE */