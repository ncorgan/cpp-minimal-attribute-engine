/*
 * Copyright (c) 2017,2019 Nicholas Corgan (n.corgan@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */

#ifndef INCLUDED_ATTRIBUTE_ENGINE_HPP
#define INCLUDED_ATTRIBUTE_ENGINE_HPP

#include <functional>
#include <unordered_map>
#include <vector>

template <typename key_type, typename val_type>
class attribute_engine
{
    public:

        template <typename T>
        using getter_fcn = std::function<T(void)>;

        template <typename T>
        using setter_fcn = std::function<void(const T&)>;

        attribute_engine() {}
        ~attribute_engine() {};

        attribute_engine(const attribute_engine&) = default;
        attribute_engine& operator=(const attribute_engine&) = default;

#if !defined(_MSC_VER) || (_MSC_VER >= 1900)
        // Defaulted move constructors/operators aren't supported until VS2015
        attribute_engine(attribute_engine&&) = default;
        attribute_engine& operator=(attribute_engine&&) = default;
#endif

        val_type get_attribute_value(
            const key_type& attribute_name
        ) const;

        void set_attribute_value(
            const key_type& attribute_name,
            const val_type& value
        );

        void register_attribute_fcns(
            const key_type& attribute_name,
            const getter_fcn<val_type>& getter,
            const setter_fcn<val_type>& setter
        );

        void register_attribute_fcns(
            key_type&& r_attribute_name,
            getter_fcn<val_type>&& r_getter,
            setter_fcn<val_type>&& r_setter
        );

        std::vector<key_type> get_attribute_names() const;

        const std::vector<val_type>& get_attribute_valid_values(
            const key_type& attribute_name
        ) const;

        void register_attribute_valid_values(
            const key_type& attribute_name,
            const std::vector<val_type>& valid_values
        );

        void register_attribute_valid_values(
            const key_type& attribute_name,
            std::vector<val_type>&& valid_values
        );

    private:
        struct attribute_fcn_pair_t
        {
            getter_fcn<val_type> getter;
            setter_fcn<val_type> setter;
        };

        using attribute_fcn_map_t = std::unordered_map<key_type, attribute_fcn_pair_t>;
        attribute_fcn_map_t _attribute_fcn_map;

        using attribute_valid_values_t = std::unordered_map<key_type, std::vector<val_type> >;
        attribute_valid_values_t _attribute_valid_values_map;
};

#include "attribute_engine.ipp"

#endif /* INCLUDED_ATTRIBUTE_ENGINE_HPP */
