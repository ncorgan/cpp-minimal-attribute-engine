/*
 * Copyright (c) 2019 Nicholas Corgan (n.corgan@gmail.com)
 *
 * Distributed under the MIval_type License (MIval_type) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIval_type)
 */

#ifndef INCLUDED_ATTRIBUTE_ENGINE_IPP
#define INCLUDED_ATTRIBUTE_ENGINE_IPP

#include <algorithm>
#include <exception>

template <typename key_type, typename val_type>
val_type attribute_engine<key_type, val_type>::get_attribute_value(
    const key_type& attribute_name
)
{
    if(_attribute_fcn_map.count(attribute_name) == 0)
    {
        throw std::invalid_argument("Invalid attribute.");
    }

    const attribute_fcn_pair_t& attribute_fcn_pair = _attribute_fcn_map.at(attribute_name);
    if(attribute_fcn_pair.getter)
    {
        return attribute_fcn_pair.getter();
    }
    else
    {
        key_type error_message = "The attribute \"";
        error_message += attribute_name;
        error_message += "\" is write-only.";

        throw std::invalid_argument(error_message);
    }
}

template <typename key_type, typename val_type>
void attribute_engine<key_type, val_type>::set_attribute_value(
    const key_type& attribute_name,
    const val_type& value
)
{
    if(_attribute_fcn_map.count(attribute_name) == 0)
    {
        throw std::invalid_argument("Invalid attribute.");
    }

    const attribute_fcn_pair_t& attribute_fcn_pair = _attribute_fcn_map.at(attribute_name);
    if(attribute_fcn_pair.setter)
    {
        attribute_fcn_pair.setter(value);
    }
    else
    {
        key_type error_message = "The attribute \"";
        error_message += attribute_name;
        error_message += "\" is read-only.";

        throw std::invalid_argument(error_message);
    }
}

template <typename key_type, typename val_type>
void attribute_engine<key_type, val_type>::register_attribute_fcns(
    const key_type& attribute_name,
    const getter_fcn<val_type>& getter,
    const setter_fcn<val_type>& setter
)
{
    attribute_fcn_pair_t attribute_fcn_pair = {getter, setter};

    _attribute_fcn_map.emplace(
        std::move(std::string(attribute_name)),
        std::move(attribute_fcn_pair)
    );
}

template <typename key_type, typename val_type>
void attribute_engine<key_type, val_type>::register_attribute_fcns(
    key_type&& r_attribute_name,
    getter_fcn<val_type>&& r_getter,
    setter_fcn<val_type>&& r_setter
)
{
    attribute_fcn_pair_t attribute_fcn_pair =
    {
        std::move(r_getter),
        std::move(r_setter)
    };

    _attribute_fcn_map.emplace(
        std::move(r_attribute_name),
        std::move(attribute_fcn_pair)
    );
}

template <typename key_type, typename val_type>
std::vector<key_type> attribute_engine<key_type, val_type>::get_attribute_names()
{
    std::vector<key_type> ret;
    for(const auto& attribute_iter: _attribute_fcn_map)
    {
        ret.emplace_back(attribute_iter.first);
    }
    std::sort(ret.begin(), ret.end());

    return ret;
}

template <typename key_type, typename val_type>
const std::vector<val_type>& attribute_engine<key_type, val_type>::get_attribute_valid_values(
    const key_type& attribute_name
)
{
    if(_attribute_fcn_map.count(attribute_name) == 0)
    {
        throw std::invalid_argument("Invalid attribute.");
    }
    else if(_attribute_valid_values_map.count(attribute_name) == 0)
    {
        throw std::invalid_argument("No valid values registered.");
    }
    else
    {
        return _attribute_valid_values_map.at(attribute_name);
    }
}

template <typename key_type, typename val_type>
void attribute_engine<key_type, val_type>::register_attribute_valid_values(
    const key_type& attribute_name,
    const std::vector<val_type>& valid_values
)
{
    register_attribute_valid_values(
        attribute_name,
        std::move(std::vector<val_type>(valid_values))
    );
}

template <typename key_type, typename val_type>
void attribute_engine<key_type, val_type>::register_attribute_valid_values(
    const key_type& attribute_name,
    std::vector<val_type>&& valid_values
)
{
    _attribute_valid_values_map.emplace(
        std::move(key_type(attribute_name)),
        std::move(valid_values)
    );
}

#endif /* INCLUDED_ATTRIBUTE_ENGINE_IPP */
