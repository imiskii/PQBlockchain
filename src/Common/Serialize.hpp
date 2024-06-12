/**
 * @file Serialize.hpp
 * @author Michal Ľaš
 * @brief Simple strucutre data serialization
 * @date 2024-03-11
 * 
 * @copyright Copyright (c) 2024
 * 
 */


#pragma once

#include <cstring>
#include "PQBtypedefs.hpp"

namespace PQB{


/**
 * @brief Serialize given field into a buffer
 * 
 * @tparam T Type of field
 * @param buffer Buffer of serialized bytes
 * @param offset Offset to buffer where place next field. This parameter is increased by number of field size in bytes.
 * @param field Field to place into the buffer
 */
template<typename T>
void serializeField(byteBuffer &buffer, size_t &offset, const T &field){
    std::memcpy(buffer.data() + offset, &field, sizeof(field));
    offset += sizeof(field);
}


/**
 * @brief Deserialize given field from a buffer
 * 
 * @tparam T Type of field
 * @param buffer Buffer of serialized bytes
 * @param offset Offset to field. This parameter is increased by number of field size in bytes.
 * @param field Field to deserialize
 */
template<typename T>
void deserializeField(const byteBuffer &buffer, size_t &offset, T &field){
    std::memcpy(&field, buffer.data() + offset, sizeof(field));
    offset += sizeof(field);
}


} // namespace PQB

/* END OF FILE */