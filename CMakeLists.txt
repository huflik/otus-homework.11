cmake_minimum_required(VERSION 3.15)

set(PATCH_VERSION "1" CACHE INTERNAL "Patch version")
set(PROJECT_VERSION 0.0.${PATCH_VERSION})

project(join_server VERSION ${PROJECT_VERSION})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -g")

add_compile_options(-Wall -Wextra -Wpedantic -Wno-unused-parameter)

find_package(Boost 1.65.0 COMPONENTS system REQUIRED)

set(SQLITE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/dms")

add_library(sqlite STATIC ${SQLITE_DIR}/sqlite3.c)
target_include_directories(sqlite PUBLIC ${SQLITE_DIR})

add_subdirectory(src)

set(CPACK_GENERATOR "DEB")
set(CPACK_PACKAGE_VENDOR "oflika256")
set(CPACK_PACKAGE_CONTACT "oflika256@gmail.com")
set(CPACK_PACKAGE_VERSION_MAJOR 0)
set(CPACK_PACKAGE_VERSION_MINOR 0)
set(CPACK_PACKAGE_VERSION_PATCH ${PATCH_VERSION})
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "oflika256 <oflika256@gmail.com>")
set(CPACK_PACKAGE_NAME "join_server")
set(CPACK_DEBIAN_PACKAGE_DESCRIPTION "An asynchronous server that performs intersection and symmetric difference operations on sets")

include(CPack)