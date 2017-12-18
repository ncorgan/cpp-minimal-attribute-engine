/*
 * Copyright (c) 2017 Nicholas Corgan (n.corgan@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */

#ifndef INCLUDED_ATTRIBUTE_ENGINE_HPP
#define INCLUDED_ATTRIBUTE_ENGINE_HPP

#include <algorithm>
#include <exception>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

template <typename T>
using getter_fcn = std::function<T(void)>;

template <typename T>
using setter_fcn = std::function<void(T)>;

template <typename T>
class attribute_engine
{
    public:
        attribute_engine() {}
        ~attribute_engine() {};

        attribute_engine(const attribute_engine&) = default;
        attribute_engine(attribute_engine&&) = default;

        attribute_engine& operator=(const attribute_engine&) = default;
        attribute_engine& operator=(attribute_engine&&) = default;

        T get_attribute_value(
            const std::string& attribute_name
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
                std::string error_message = "The attribute \"";
                error_message += attribute_name;
                error_message += "\" is write-only.";

                throw std::invalid_argument(error_message);
            }
        }

        void set_attribute_value(
            const std::string& attribute_name,
            const T& value
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
                std::string error_message = "The attribute \"";
                error_message += attribute_name;
                error_message += "\" is read-only.";

                throw std::invalid_argument(error_message);
            }
        }

        void register_attribute_fcns(
            const std::string& attribute_name,
            const getter_fcn<T>& getter,
            const setter_fcn<T>& setter
        )
        {
            attribute_fcn_pair_t attribute_fcn_pair = {getter, setter};

            _attribute_fcn_map.emplace(
                attribute_name,
                attribute_fcn_pair
            );
        }

        std::vector<std::string> get_attribute_names()
        {
            std::vector<std::string> ret;
            for(const auto& attribute_iter: _attribute_fcn_map)
            {
                ret.emplace_back(attribute_iter.first);
            }
            std::sort(ret.begin(), ret.end());

            return ret;
        }

        const std::vector<T>& get_attribute_valid_values(
            const std::string& attribute_name
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

        void register_attribute_valid_values(
            const std::string& attribute_name,
            const std::vector<T>& valid_values
        )
        {
            register_attribute_valid_values(
                attribute_name,
                std::move(std::vector<T>(valid_values))
            );
        }

        void register_attribute_valid_values(
            const std::string& attribute_name,
            std::vector<T>&& valid_values
        )
        {
            _attribute_valid_values_map.emplace(
                std::move(std::string(attribute_name)),
                std::move(valid_values)
            );
        }

    private:
        struct attribute_fcn_pair_t
        {
            getter_fcn<T> getter;
            setter_fcn<T> setter;
        };

        using attribute_fcn_map_t = std::unordered_map<std::string, attribute_fcn_pair_t>;
        attribute_fcn_map_t _attribute_fcn_map;

        using attribute_valid_values_t = std::unordered_map<std::string, std::vector<T> >;
        attribute_valid_values_t _attribute_valid_values_map;
};

#endif /* INCLUDED_ATTRIBUTE_ENGINE_HPP */
