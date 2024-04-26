cmake_minimum_required(VERSION 3.28.0)
project(WikiSearch VERSION 0.1.0 LANGUAGES C CXX)

set(EXEC_NAME WikiSearch)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SRC_FILES "src/*.cpp" "src/*.c")

include(FetchContent)

FetchContent_Declare(
  Boost
  URL https://github.com/boostorg/boost/releases/download/boost-1.84.0/boost-1.84.0.tar.xz
  URL_MD5 893b5203b862eb9bbd08553e24ff146a
  DOWNLOAD_EXTRACT_TIMESTAMP ON
)

set(BOOST_INCLUDE_LIBRARIES asio beast)
FetchContent_MakeAvailable(Boost)

add_subdirectory(dependencies/libexpat/expat)

add_executable(WikiSearch ${SRC_FILES})

if(MINGW)
  target_link_libraries(WikiSearch PRIVATE ws2_32 wsock32)
endif()

# Link against expat library
target_link_libraries(WikiSearch PRIVATE expat Boost::beast Boost::asio)

# Set include directories.
target_include_directories(WikiSearch PRIVATE dependencies/libexpat)
