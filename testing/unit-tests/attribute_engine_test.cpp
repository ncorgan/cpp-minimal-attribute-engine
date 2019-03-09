/*
 * Copyright (c) 2017,2019 Nicholas Corgan (n.corgan@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */

#include "attribute_engine.hpp"

#include <gtest/gtest.h>

#include <functional>
#include <vector>

// Minimal test functions

static inline int int_getter()
{
    return 5;
}

static inline void int_setter(int num)
{
    (void)num;
}

// Test class

class attribute_test_class
{
    public:
        attribute_test_class():
            _string_attribute_engine(),
            _string_vector({"A","B"})
        {
            _register_attributes();
        }

        std::string get_string_attribute(
            const std::string& attribute_name
        )
        {
            return _string_attribute_engine.get_attribute_value(
                       attribute_name
                   );
        }

        void set_string_attribute(
            const std::string& attribute_name,
            const std::string& value
        )
        {
            _string_attribute_engine.set_attribute_value(
                attribute_name,
                value
            );
        }

    private:
        attribute_engine<std::string, std::string> _string_attribute_engine;

        std::vector<std::string> _string_vector;

        std::string _get_string_from_vector(
            size_t index
        )
        {
            return _string_vector.at(index);
        }

        void _set_string_in_vector(
            size_t index,
            const std::string& value
        )
        {
            _string_vector.at(index) = value;
        }

        void _register_attributes()
        {
            using namespace std::placeholders;

            _string_attribute_engine.register_attribute_fcns(
                "First string",
                std::bind(
                    &attribute_test_class::_get_string_from_vector,
                    this,
                    0
                ),
                std::bind(
                    &attribute_test_class::_set_string_in_vector,
                    this,
                    0,
                    _1
                )
            );
            _string_attribute_engine.register_attribute_fcns(
                "Second string",
                std::bind(
                    &attribute_test_class::_get_string_from_vector,
                    this,
                    1
                ),
                std::bind(
                    &attribute_test_class::_set_string_in_vector,
                    this,
                    1,
                    _1
                )
            );
        }
};

TEST(cpp_attribute_test, test_registering)
{
    attribute_engine<std::string, int> int_attribute_engine;

    int_attribute_engine.register_attribute_fcns(
        "Attribute 1",
        int_getter,
        int_setter
    );

    EXPECT_EQ(5, int_attribute_engine.get_attribute_value("Attribute 1"));
    int_attribute_engine.set_attribute_value("Attribute 1", 0);

    // Test a read-only attribute.
    int_attribute_engine.register_attribute_fcns(
        "Attribute 2",
        int_getter,
        nullptr // No setter
    );

    EXPECT_EQ(5, int_attribute_engine.get_attribute_value("Attribute 2"));
    EXPECT_THROW(
        int_attribute_engine.set_attribute_value("Attribute 2", 0);
    , std::invalid_argument);

    // Test a write-only attribute.
    int_attribute_engine.register_attribute_fcns(
        "Attribute 3",
        nullptr, // No getter
        int_setter
    );

    EXPECT_THROW(
        (void)int_attribute_engine.get_attribute_value("Attribute 3");
    , std::invalid_argument);
    int_attribute_engine.set_attribute_value("Attribute 3", 0);

    // TODO: test overriding existing attribute
}

TEST(cpp_attribute_test, test_get_attribute_names)
{
    attribute_engine<std::string, std::string> string_attribute_engine;

    // Register some attributes that don't do anything for the sake of test.
    string_attribute_engine.register_attribute_fcns(
        "Attribute 1",
        nullptr,
        nullptr
    );
    string_attribute_engine.register_attribute_fcns(
        "Attribute 2",
        nullptr,
        nullptr
    );
    string_attribute_engine.register_attribute_fcns(
        "Attribute 3",
        nullptr,
        nullptr
    );

    const std::vector<std::string> expected_names =
    {
        "Attribute 1",
        "Attribute 2",
        "Attribute 3"
    };

    EXPECT_EQ(expected_names, string_attribute_engine.get_attribute_names());
}

TEST(cpp_attribute_test, test_valid_values)
{
    attribute_engine<std::string, int> int_attribute_engine;
    const std::string attribute_name = "Attribute";
    const std::vector<int> valid_values = {1,2,3,4,5};

    // Attempting to get valid values when the attribute does not exist
    // should throw.
    EXPECT_THROW(
        (void)int_attribute_engine.get_attribute_valid_values(attribute_name);
    , std::invalid_argument);

    int_attribute_engine.register_attribute_fcns(
        attribute_name,
        int_getter,
        int_setter
    );

    // Attempting to get valid values when the attribute exists but does
    // not have registered value values should throw.
    EXPECT_THROW(
        (void)int_attribute_engine.get_attribute_valid_values(attribute_name);
    , std::invalid_argument);

    int_attribute_engine.register_attribute_valid_values(
        attribute_name,
        valid_values
    );
    EXPECT_EQ(
        valid_values,
        int_attribute_engine.get_attribute_valid_values(attribute_name)
    );
}

TEST(cpp_attribute_test, test_binding_class_functions)
{
    attribute_test_class test_class;

    // Test default values.
    EXPECT_EQ("A", test_class.get_string_attribute("First string"));
    EXPECT_EQ("B", test_class.get_string_attribute("Second string"));

    // Set values.
    test_class.set_string_attribute("First string", "C");
    test_class.set_string_attribute("Second string", "D");

    // Test new values.
    EXPECT_EQ("C", test_class.get_string_attribute("First string"));
    EXPECT_EQ("D", test_class.get_string_attribute("Second string"));
}
