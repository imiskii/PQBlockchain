/**
 * @file Serialize.cpp
 * @author Michal Ľaš
 * @brief Simple strucutre data serialization
 * @date 2024-03-11
 * 
 * @copyright Copyright (c) 2024
 * 
 */


#include "Serialize.hpp"


namespace PQB{

template<typename T>
void serializeField(byteBuffer &buffer, size_t &offset, const T &field);

template<typename T>
void deserializeField(const byteBuffer &buffer, size_t &offset, T &field);


} // namespace PQB

/* END OF FILE */