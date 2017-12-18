/*
 * Copyright (c) 2017 Nicholas Corgan (n.corgan@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */

#include "attribute_engine.hpp"

#include <gtest/gtest.h>

static inline int int_getter()
{
    return 5;
}

static inline void int_setter(int num)
{
    (void)num;
}

TEST(cpp_attribute_test, test_registering)
{
    attribute_engine<int> int_attribute_engine;

    int_attribute_engine.register_attribute_fcns(
        "Attribute 1",
        int_getter,
        int_setter
    );

    EXPECT_EQ(5, int_attribute_engine.get_attribute("Attribute 1"));
    int_attribute_engine.set_attribute("Attribute 1", 0);

    // Test a read-only attribute.
    int_attribute_engine.register_attribute_fcns(
        "Attribute 2",
        int_getter,
        nullptr // No setter
    );

    EXPECT_EQ(5, int_attribute_engine.get_attribute("Attribute 2"));
    EXPECT_THROW(
        int_attribute_engine.set_attribute("Attribute 2", 0);
    , std::invalid_argument);

    // Test a write-only attribute.
    int_attribute_engine.register_attribute_fcns(
        "Attribute 3",
        nullptr, // No getter
        int_setter
    );

    EXPECT_THROW(
        (void)int_attribute_engine.get_attribute("Attribute 3");
    , std::invalid_argument);
    int_attribute_engine.set_attribute("Attribute 3", 0);

    // TODO: test overriding existing attribute
}

TEST(cpp_attribute_test, test_get_attribute_names)
{
    attribute_engine<std::string> string_attribute_engine;

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
    attribute_engine<int> int_attribute_engine;
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
