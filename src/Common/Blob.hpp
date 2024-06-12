/**
 * @file Blob.hpp
 * @author Michal Ľaš
 * @brief Implementation of blob and other large datatypes
 * @date 2024-02-10
 * 
 * @copyright Copyright (c) 2024
 * 
 * 
 * This code is inspired by implementation from Bitcoin Core project
 * 
 * Taken from: Bitcoin Core project (Distributed under MIT licence)
 * @author Satoshi Nakamoto, The Bitcoin Core developers
 * 
 * Availability: https://github.com/bitcoin/bitcoin
 * 
 * @copyright Copyright (c) 2009-2010
 * @copyright Copyright (c) 2009-2022
 * 
 */


// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include <span>
#include <algorithm>
#include <array>
#include <cassert>
#include <cstring>
#include <stdint.h>
#include <string>

#include "HexBase.hpp"

/**
 * @brief Template base class for fixed-sized opaque blobs.
 * 
 * @tparam BITS size of the blob in bits
 * 
 */
template<unsigned int BITS>
class base_blob
{
protected:
    static constexpr int WIDTH = BITS / 8;
    static_assert(BITS % 8 == 0, "base_blob currently only supports whole bytes.");
    std::array<uint8_t, WIDTH> m_data;
    static_assert(WIDTH == sizeof(m_data), "Sanity check");

public:
    /* construct 0 value by default */
    constexpr base_blob() : m_data() {}

    /* constructor for constants between 1 and 255 */
    constexpr explicit base_blob(uint8_t v) : m_data{v} {}

    constexpr explicit base_blob(std::span<const unsigned char> vch)
    {
        assert(vch.size() == WIDTH);
        std::copy(vch.begin(), vch.end(), m_data.begin());
    }

    constexpr bool IsNull() const
    {
        return std::all_of(m_data.begin(), m_data.end(), [](uint8_t val) {
            return val == 0;
        });
    }

    constexpr void SetNull()
    {
        std::fill(m_data.begin(), m_data.end(), 0);
    }

    constexpr int Compare(const base_blob& other) const { return std::memcmp(m_data.data(), other.m_data.data(), WIDTH); }

    friend constexpr bool operator==(const base_blob& a, const base_blob& b) { return a.Compare(b) == 0; }
    friend constexpr bool operator!=(const base_blob& a, const base_blob& b) { return a.Compare(b) != 0; }
    friend constexpr bool operator<(const base_blob& a, const base_blob& b) { return a.Compare(b) < 0; }

    constexpr const unsigned char* data() const { return m_data.data(); }
    constexpr unsigned char* data() { return m_data.data(); }

    constexpr unsigned char* begin() { return m_data.data(); }
    constexpr unsigned char* end() { return m_data.data() + WIDTH; }

    constexpr const unsigned char* begin() const { return m_data.data(); }
    constexpr const unsigned char* end() const { return m_data.data() + WIDTH; }

    static constexpr unsigned int size() { return WIDTH; }

    /**
     * @brief Get the Hex representation of blob
     * 
     * @return std::string bytes in hexadecimal format
     */
    std::string getHex() const;

    /**
     * @brief Set the base_blob on given hexadecimal argument
     * 
     * @param str hexadecimal string
     */
    void setHex(const std::string &str);
};


/**
 * @brief 512-bit opaque blob.
 * 
 */
class byte64_t : public base_blob<512> {
public:
    constexpr byte64_t() = default;
    constexpr explicit byte64_t(uint8_t v) : base_blob<512>(v) {}
    constexpr explicit byte64_t(std::span<const unsigned char> vch) : base_blob<512>(vch) {}
};