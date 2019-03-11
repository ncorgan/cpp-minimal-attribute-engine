/*
 * Copyright (c) 2017,2019 Nicholas Corgan (n.corgan@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */

#include "attribute_engine/attribute_engine.hpp"

#include <gtest/gtest.h>

#ifdef TEST_BOOST_FUNCTION
#include <boost/bind.hpp>
#include <boost/function.hpp>
#endif

#include <cstdint>
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

///////////////
// OLD ABOVE //
///////////////

// Use SFINAE magic to add test functions for different types.
// TODO: add function to get valid/invalid values to migrate remaining test.

template <typename return_type, typename test_type>
using return_if_type_is_enum_or_numeric = typename
                                          std::enable_if<
                                              std::is_enum<test_type>::value ||
                                              std::is_arithmetic<test_type>::value,
                                              return_type
                                          >::type;

template <typename key_type>
return_if_type_is_enum_or_numeric<void, key_type> get_test_keys(
    key_type* p_key1_out,
    key_type* p_key2_out,
    key_type* p_key3_out
)
{
    ASSERT_NE(nullptr, p_key1_out);
    ASSERT_NE(nullptr, p_key2_out);
    ASSERT_NE(nullptr, p_key3_out);

    (*p_key1_out) = static_cast<key_type>(0);
    (*p_key2_out) = static_cast<key_type>(1);
    (*p_key3_out) = static_cast<key_type>(2);
}

template <typename key_type>
detail::return_if_types_match<void, key_type, std::string> get_test_keys(
    key_type* p_key1_out,
    key_type* p_key2_out,
    key_type* p_key3_out
)
{
    ASSERT_NE(nullptr, p_key1_out);
    ASSERT_NE(nullptr, p_key2_out);
    ASSERT_NE(nullptr, p_key3_out);

    p_key1_out->assign("Key 1");
    p_key2_out->assign("Key 2");
    p_key3_out->assign("Key 3");
}

enum test_enum
{
    VAL0 = 0,
    VAL1,
    VAL2
};

enum class test_enum_class: unsigned short
{
    CLASS_VAL0 = 0,
    CLASS_VAL1,
    CLASS_VAL2
};

template <typename key_type>
class attribute_test: public ::testing::Test
{
    public:

        using val_type = std::string;

        static key_type key1;
        static key_type key2;
        static key_type key3;

        static const val_type test_value;
        static val_type static_value;

        attribute_engine<key_type, val_type> engine;
        val_type var_in_class;

        virtual void TearDown()
        {
            static_value.clear();
        }

        static void SetUpTestCase()
        {
            ASSERT_NO_FATAL_FAILURE(get_test_keys<key_type>(
                &key1,
                &key2,
                &key3
            ));
        }

        val_type class_getter()
        {
            return var_in_class;
        }

        void class_setter(const val_type& val)
        {
            var_in_class = val;
        }

        // Make this static so the function signature is val_type(void) instead
        // of val_type(attribute_test<key_type>).
        static val_type static_getter()
        {
            return static_value;
        }

        // Make this static so the function signature is void(const val_type&)
        // instead of void(attribute_test<key_type>, const val_type&).
        static void static_setter(const val_type& val)
        {
            static_value = val;
        }

        void test_set_and_get(const key_type& key)
        {
            engine.set_attribute_value(key, test_value);
            ASSERT_EQ(test_value, engine.get_attribute_value(key));
        }
};

template <typename key_type>
key_type attribute_test<key_type>::key1;

template <typename key_type>
key_type attribute_test<key_type>::key2;

template <typename key_type>
key_type attribute_test<key_type>::key3;

template <typename key_type>
const typename attribute_test<key_type>::val_type attribute_test<key_type>::test_value("ABCDEFG");

template <typename key_type>
typename attribute_test<key_type>::val_type attribute_test<key_type>::static_value;

// TODO: parameterize enum testing based on compiler support
using key_types = ::testing::Types<
    char,
    uint8_t,
    uint16_t,
    uint32_t,
    uint64_t,
    int8_t,
    int16_t,
    int32_t,
    int64_t,
#if 0
    test_enum,
    test_enum_class,
#endif
    std::string
>;
TYPED_TEST_CASE(attribute_test, key_types);

TYPED_TEST(attribute_test, set_and_get_with_function_params)
{
    using key_type = TypeParam;
    using test_class = attribute_test<TypeParam>;

    const key_type& key = test_class::key1;

    this->engine.register_attribute_fcns(
        key,
        &test_class::static_getter,
        &test_class::static_setter
    );

    ASSERT_NO_FATAL_FAILURE(this->test_set_and_get(key));

    // Sanity check.
    ASSERT_EQ(test_class::test_value, test_class::static_value);
}

TYPED_TEST(attribute_test, set_and_get_with_lambdas)
{
    using key_type = TypeParam;
    using test_class = attribute_test<TypeParam>;
    using val_type = typename test_class::val_type;

    const key_type& key = test_class::key1;

    // The lambdas will put the values here.
    val_type var_in_test_body;

    this->engine.register_attribute_fcns(
        key,
        [&var_in_test_body]() { return var_in_test_body; },
        [&var_in_test_body](const val_type& val) { var_in_test_body = val; }
    );

    ASSERT_NO_FATAL_FAILURE(this->test_set_and_get(key));

    // Sanity check.
    ASSERT_EQ(test_class::test_value, var_in_test_body);
}

TYPED_TEST(attribute_test, set_and_get_with_bound_class_functions)
{
    using key_type = TypeParam;
    using test_class = attribute_test<TypeParam>;

    const key_type& key = test_class::key1;

    this->engine.register_attribute_fcns(
        key,
        std::bind(
            &test_class::class_getter,
            this
        ),
        std::bind(
            &test_class::class_setter,
            this,
            std::placeholders::_1
        )
    );

    ASSERT_NO_FATAL_FAILURE(this->test_set_and_get(key));

    // Sanity check.
    ASSERT_EQ(test_class::test_value, this->var_in_class);
}

TYPED_TEST(attribute_test, set_and_get_with_function_pointers)
{
    using key_type = TypeParam;
    using test_class = attribute_test<TypeParam>;
    using val_type = typename test_class::val_type;

    const key_type& key = test_class::key1;

    // This will point to the same functions as set_and_get_with_function_params,
    // but this test explicitly passes in caller-created function pointers.
    val_type (*getter_fptr)(void) = &test_class::static_getter;
    void (*setter_fptr)(const val_type&) = &test_class::static_setter;

    this->engine.register_attribute_fcns(
        key,
        getter_fptr,
        setter_fptr
    );

    ASSERT_NO_FATAL_FAILURE(this->test_set_and_get(key));

    // Sanity check.
    ASSERT_EQ(test_class::test_value, test_class::static_value);
}

#ifdef TEST_BOOST_FUNCTION
TYPED_TEST(attribute_test, set_and_get_with_boost_function)
{
    using key_type = TypeParam;
    using test_class = attribute_test<TypeParam>;
    using val_type = typename test_class::val_type;

    const key_type& key = test_class::key1;

    // This will point to the same functions as set_and_get_with_function_params,
    // but this test explicitly passes in caller-created boost::functions.
    boost::function<val_type(void)> boost_getter_fcn = &test_class::static_getter;
    boost::function<void(val_type)> boost_setter_fcn = &test_class::static_setter;

    this->engine.register_attribute_fcns(
        key,
        boost_getter_fcn,
        boost_setter_fcn
    );

    ASSERT_NO_FATAL_FAILURE(this->test_set_and_get(key));

    // Sanity check.
    ASSERT_EQ(test_class::test_value, test_class::static_value);
}

TYPED_TEST(attribute_test, set_and_get_with_boost_bind)
{
    using key_type = TypeParam;
    using test_class = attribute_test<TypeParam>;
    using val_type = typename test_class::val_type;

    const key_type& key = test_class::key1;

    // This will point to the same functions as set_and_get_with_function_params,
    // but this test explicitly passes in caller-created boost::binds.
    this->engine.register_attribute_fcns(
        key,
        boost::bind(&test_class::static_getter),
        boost::bind(&test_class::static_setter, _1)
    );

    ASSERT_NO_FATAL_FAILURE(this->test_set_and_get(key));

    // Sanity check.
    ASSERT_EQ(test_class::test_value, test_class::static_value);
}
#endif
