/*
 * Copyright (c) 2017,2019 Nicholas Corgan (n.corgan@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */

#include "attribute_engine.hpp"

#include <gtest/gtest.h>

#ifdef TEST_BOOST_FUNCTION
#include <boost/bind.hpp>
#include <boost/function.hpp>
#endif

#include <functional>
#include <vector>

// Minimal test functions

static inline int getter_fcn()
{
    return 5;
}

static inline void setter_fcn(int num)
{
    (void)num;
}

// Test class containing an attribute engine

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
            using std::placeholders::_1;

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
        getter_fcn,
        setter_fcn
    );

    EXPECT_EQ(5, int_attribute_engine.get_attribute_value("Attribute 1"));
    int_attribute_engine.set_attribute_value("Attribute 1", 0);

    // Test a read-only attribute.
    int_attribute_engine.register_attribute_fcns(
        "Attribute 2",
        getter_fcn,
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
        setter_fcn
    );

    EXPECT_THROW(
        (void)int_attribute_engine.get_attribute_value("Attribute 3");
    , std::invalid_argument);
    int_attribute_engine.set_attribute_value("Attribute 3", 0);

    // Test registering by moving parameters.
    auto getter_to_move = &getter_fcn;
    auto setter_to_move = &setter_fcn;

    const std::string attribute_name("Attribute 4");
    std::string attribute_name_to_move(attribute_name);

    int_attribute_engine.register_attribute_fcns(
        std::move(attribute_name_to_move),
        std::move(getter_to_move),
        std::move(setter_to_move)
    );

    EXPECT_EQ(5, int_attribute_engine.get_attribute_value(attribute_name));
    int_attribute_engine.set_attribute_value(attribute_name, 0);

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
        getter_fcn,
        setter_fcn
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

TEST(cpp_attribute_test, test_registering_lambdas)
{
    struct
    {
        int dummy_field;
    } dummy_struct;
    dummy_struct.dummy_field = 0;

    attribute_engine<std::string, int> engine;

    const std::string attribute_name("foo");
    const int attribute_value = 5;

    engine.register_attribute_fcns(
        attribute_name,
        [&dummy_struct]() {return dummy_struct.dummy_field; },
        [&dummy_struct](int var) { dummy_struct.dummy_field = var; }
    );

    engine.set_attribute_value(attribute_name, attribute_value);
    ASSERT_EQ(attribute_value, engine.get_attribute_value(attribute_name));
}

TEST(cpp_attribute_test, test_registering_function_pointers)
{
    attribute_engine<std::string, int> engine;

    const std::string attribute_name("foo");

    int (*getter_fptr)(void) = &getter_fcn;
    void (*setter_fptr)(int) = &setter_fcn;

    engine.register_attribute_fcns(
        attribute_name,
        getter_fptr,
        setter_fptr
    );
}

#ifdef TEST_BOOST_FUNCTION
TEST(cpp_attribute_test, test_boost_function)
{
    attribute_engine<std::string, int> engine;

    const std::string attribute_name("foo");

    boost::function<int(void)> boost_getter_fcn = &getter_fcn;
    boost::function<void(int)> boost_setter_fcn = &setter_fcn;

    engine.register_attribute_fcns(
        attribute_name,
        boost_getter_fcn,
        boost_setter_fcn
    );
}

int getter_for_bind(int int1)
{
    (void)int1;
    return getter_fcn();
}

void setter_for_bind(int int1, int int2)
{
    (void)int1;
    setter_fcn(int2);
}

TEST(cpp_attribute_test, test_boost_bind)
{
    attribute_engine<std::string, int> engine;

    const int int_param = 5;

    auto bound_getter = boost::bind(&getter_for_bind, int_param);
    auto bound_setter = boost::bind(&setter_for_bind, int_param, _1);

    const std::string attribute_name("foo");
    const int attribute_value = 5;

    engine.register_attribute_fcns(
        attribute_name,
        bound_getter,
        bound_setter
    );

    engine.set_attribute_value(attribute_name, attribute_value);
    ASSERT_EQ(attribute_value, engine.get_attribute_value(attribute_name));
}
#endif
