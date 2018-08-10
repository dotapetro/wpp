cmake_minimum_required(VERSION 3.12)
project(NetUtils)
set(CMAKE_CXX_STANDARD 17)

set (CMAKE_CXX_FLAGS -DBOOST_ERROR_CODE_HEADER_ONLY )

# Pthread
find_package (Threads)

# OpenSSL
find_package(OpenSSL)

# Boost
find_package( Boost COMPONENTS thread system regex REQUIRED )
include_directories(${Boost_INCLUDE_DIR})

# Main
add_executable(NetUtils main.cpp client/connection_pool.h client/parse_options.h client/tag.h client/parser.h)

# Pthread
target_link_libraries (NetUtils ${CMAKE_THREAD_LIBS_INIT})

# SSL
target_link_libraries(NetUtils ${OPENSSL_LIBRARIES})

# Boost
link_libraries(NetUtils LINK_PUBLIC ${Boost_LIBRARIES})