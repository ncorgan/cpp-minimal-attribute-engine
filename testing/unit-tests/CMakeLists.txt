#
# Copyright (c) 2017 Nicholas Corgan (n.corgan@gmail.com)
#
# Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
# or copy at http://opensource.org/licenses/MIT)
#

# Build GoogleTest
ADD_SUBDIRECTORY(googletest/googletest)

ADD_EXECUTABLE(attribute_engine_test attribute_engine_test.cpp)
TARGET_LINK_LIBRARIES(attribute_engine_test gtest gtest_main)
