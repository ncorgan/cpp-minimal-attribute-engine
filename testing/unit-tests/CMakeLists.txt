#
# Copyright (c) 2017,2019 Nicholas Corgan (n.corgan@gmail.com)
#
# Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
# or copy at http://opensource.org/licenses/MIT)
#

# Build GoogleTest
SET(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
ADD_SUBDIRECTORY(googletest/googletest)

IF(Boost_FOUND)
    ADD_DEFINITIONS(-DTEST_BOOST_FUNCTION)
ENDIF()

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/include)

ADD_EXECUTABLE(attribute_engine_test attribute_engine_test.cpp)
TARGET_LINK_LIBRARIES(attribute_engine_test gtest gtest_main)

#
# For GCC or Clang, if the caller specified a standard, override the
# default (C++11).
#
IF(NOT "$ENV{STD}" STREQUAL "")
    SET(CPP_STD "$ENV{STD}" CACHE INTERNAL "C++ standard")
ELSEIF(MSVC)
    # This is the first version supported by this flag.
    SET(CPP_STD "c++14" CACHE INTERNAL "C++ standard")
ELSE()
    SET(CPP_STD "c++11" CACHE INTERNAL "C++ standard")
ENDIF()

IF(WIN32)
    IF(MINGW)
        SET_SOURCE_FILES_PROPERTIES(attribute_engine_test.cpp
            PROPERTIES COMPILE_FLAGS "-std=${CPP_STD}"
        )
    ELSE()
        # The /std: flag was added in MSVC 14.0
        IF(MSVC AND (MSVC_VERSION GREATER 1800))
            SET_SOURCE_FILES_PROPERTIES(attribute_engine_test.cpp
                PROPERTIES COMPILE_FLAGS "/MP /EHsc /std:${CPP_STD}"
            )
        ELSE()
            SET_SOURCE_FILES_PROPERTIES(attribute_engine_test.cpp
                PROPERTIES COMPILE_FLAGS "/MP /EHsc"
            )
        ENDIF()
    ENDIF()

    ADD_TEST(
        attribute_engine_test
        ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/attribute_engine_test.exe
    )
ELSE()
    SET_SOURCE_FILES_PROPERTIES(attribute_engine_test.cpp
        PROPERTIES COMPILE_FLAGS "-std=${CPP_STD}"
    )

    ADD_TEST(
        attribute_engine_test
        ${CMAKE_CURRENT_BINARY_DIR}/attribute_engine_test
    )
ENDIF()
