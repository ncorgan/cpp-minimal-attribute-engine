C++ Minimal Attribute Engine (WIP)
===============================================

[![license](https://img.shields.io/badge/license-MIT-blue.svg)](https://github.com/ncorgan/cpp-minimal-attribute-engine/blob/master/LICENSE.txt)
[![Travis-CI Build](https://api.travis-ci.org/ncorgan/cpp-minimal-attribute-engine.svg)](https://travis-ci.org/ncorgan/cpp-minimal-attribute-engine)
[![AppVeyor Build](https://ci.appveyor.com/api/projects/status/github/ncorgan/cpp-minimal-attribute-engine)](https://ci.appveyor.com/project/ncorgan/cpp-minimal-attribute-engine)

This class is a header-only templated attribute engine, in which
callers and setters are registered.

Supported Platforms
-------------------------------------

This class should be able to be used with any compiler that supports C++11.

Using [Travis](https://travis-ci.org/ncorgan/libpkmn) and
[AppVeyor](https://ci.appveyor.com/project/ncorgan/libpkmn), every push to this
repository is automatically tested under the following configurations:

 * Ubuntu 14.04 (x64)
   * GCC: 4.8.4, 5.4.1, 6.2.0, 7.2.0
   * Clang: 3.9.0, 4.0.1, 5.0.0
 * OS X 10.12.6
   * AppleClang: 8.1.0.8020042
 * Windows Server 2012 R2 (x64)
   * MSVC: 2013, 2015 (Win32, x64)
 * Windows Server 2016 (x64)
   * MSVC: 2017 (Win32, x64)

Testing and support for more configurations is in progress.

README last updated: 2017/12/17
