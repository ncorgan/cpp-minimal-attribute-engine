/*
 * Copyright (c) 019 Nicholas Corgan (n.corgan@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */

#ifndef INCLUDED_ATTRIBUTE_ENGINE_DETAIL_HPP
#define INCLUDED_ATTRIBUTE_ENGINE_DETAIL_HPP

#include <sstream>
#include <string>
#include <type_traits>

namespace detail
{
    // SFINAE

    template <typename return_type, typename test_type>
    using return_if_type_is_enum = typename std::enable_if<std::is_enum<test_type>::value, return_type>::type;

    template <typename return_type, typename test_type>
    using return_if_type_is_numeric = typename std::enable_if<std::is_arithmetic<test_type>::value, return_type>::type;

    template <typename return_type, typename type1, typename type2>
    using return_if_types_match = typename std::enable_if<std::is_same<type1, type2>::value, return_type>::type;

    //
    // Common code that SFINAEs different types
    //

    template <typename key_type>
    static inline return_if_type_is_numeric<std::string, key_type> key_to_string(const key_type& key)
    {
        return std::to_string(key);
    }

    template <typename key_type>
    static inline return_if_type_is_enum<std::string, key_type> key_to_string(const key_type& key)
    {
        return std::to_string(static_cast<typename std::underlying_type<key_type>::type>(key));
    }

    template <typename key_type>
    static inline return_if_types_match<std::string, key_type, std::string> key_to_string(const key_type& key)
    {
        return key;
    }

    //
    // Message-specific functions
    //

    template <typename key_type>
    static std::string get_invalid_attribute_error_message(const key_type& key)
    {
        std::ostringstream error_stream;
        error_stream << "Invalid attribute: ";
        error_stream << key_to_string(key);

        return error_stream.str();
    }

    template <typename key_type>
    static std::string get_readonly_error_message(const key_type& key)
    {
        std::ostringstream error_stream;
        error_stream << "The given attribute is read-only: ";
        error_stream << key_to_string(key);

        return error_stream.str();
    }

    template <typename key_type>
    static std::string get_writeonly_error_message(const key_type& key)
    {
        std::ostringstream error_stream;
        error_stream << "The given attribute is write-only: ";
        error_stream << key_to_string(key);

        return error_stream.str();
    }
}

#endif /* INCLUDED_ATTRIBUTE_ENGINE_DETAIL_HPP */
