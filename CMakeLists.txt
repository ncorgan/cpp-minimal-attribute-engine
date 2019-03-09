#
# Copyright (c) 2017,2019 Nicholas Corgan (n.corgan@gmail.com)
#
# Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
# or copy at http://opensource.org/licenses/MIT)
#

########################################################################
# CMake configuration
########################################################################
CMAKE_MINIMUM_REQUIRED(VERSION 2.8.9)
PROJECT(cpp-minimal-attribute-engine CXX)

IF(NOT DEFINED CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE "Release")
ENDIF()

########################################################################
# Find Boost for boost::function testing
########################################################################
FIND_PACKAGE(Boost 1.65)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})

ENABLE_TESTING()
ADD_SUBDIRECTORY(testing/unit-tests)
