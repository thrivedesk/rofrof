cmake_minimum_required(VERSION 3.16)
project(rofrof CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# All fetched dependencies are cloned/built under ./third_party (gitignored),
# so the project builds standalone without installing system -dev packages.
# FORCE is required: include(FetchContent) seeds this cache var with a
# build/_deps default, which would otherwise win over a plain set().
set(FETCHCONTENT_BASE_DIR "${CMAKE_SOURCE_DIR}/third_party" CACHE PATH "Vendored dependency tree" FORCE)

include(FetchContent)

# --- System libraries (ubiquitous; not practical to vendor) -----------------
# OpenSSL is not a CMake project and zlib ships with every toolchain, so these
# are located rather than fetched. Both are already present on a normal box.
find_package(OpenSSL REQUIRED)
find_package(ZLIB REQUIRED)
find_package(Threads REQUIRED)

# --- jsoncpp (fetched, so no system libjsoncpp-dev is required) --------------
set(JSONCPP_WITH_TESTS OFF CACHE BOOL "" FORCE)
set(JSONCPP_WITH_POST_BUILD_UNITTEST OFF CACHE BOOL "" FORCE)
set(JSONCPP_WITH_PKGCONFIG_SUPPORT OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(BUILD_STATIC_LIBS ON CACHE BOOL "" FORCE)
set(BUILD_OBJECT_LIBS OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
    jsoncpp
    GIT_REPOSITORY https://github.com/open-source-parsers/jsoncpp.git
    GIT_TAG 1.9.5
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(jsoncpp)

# --- uSockets (C; we compile it ourselves: epoll backend + OpenSSL) ----------
# Pinned to the exact commit uWebSockets v18.15.0 references.
FetchContent_Declare(
    usockets
    GIT_REPOSITORY https://github.com/uNetworking/uSockets.git
    GIT_TAG 7dea1e00ae922f3d1bf23d53dedebdf45f21dc3b
)
FetchContent_MakeAvailable(usockets)

# --- uWebSockets (header-only) ----------------------------------------------
FetchContent_Declare(
    uwebsockets
    GIT_REPOSITORY https://github.com/uNetworking/uWebSockets.git
    GIT_TAG v18.15.0
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(uwebsockets)

# Build uSockets as a static lib. Linux epoll backend + OpenSSL crypto only;
# the libuv, GCD and wolfSSL backends are intentionally excluded.
file(GLOB USOCKETS_SOURCES
    ${usockets_SOURCE_DIR}/src/*.c
    ${usockets_SOURCE_DIR}/src/eventing/epoll_kqueue.c
    ${usockets_SOURCE_DIR}/src/crypto/openssl.c
    ${usockets_SOURCE_DIR}/src/crypto/sni_tree.cpp
)
add_library(usockets STATIC ${USOCKETS_SOURCES})
target_include_directories(usockets PUBLIC ${usockets_SOURCE_DIR}/src)
target_compile_definitions(usockets
    PUBLIC LIBUS_USE_OPENSSL LIBUS_USE_EPOLL
    PRIVATE _GNU_SOURCE
)
target_link_libraries(usockets PUBLIC OpenSSL::SSL OpenSSL::Crypto)

# --- The RofRof server ------------------------------------------------------
add_executable(rofrof src/main.cpp)
target_include_directories(rofrof PRIVATE
    ${uwebsockets_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src
)
target_compile_definitions(rofrof PRIVATE UWS_WITH_PROXY)
target_link_libraries(rofrof PRIVATE
    usockets
    jsoncpp_static
    ZLIB::ZLIB
    Threads::Threads
    OpenSSL::SSL
    OpenSSL::Crypto
)

# Emit the binary at build/rofrof (matches the README run instructions).
set_target_properties(rofrof PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
