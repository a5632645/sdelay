cmake_minimum_required(VERSION 3.22)

project(sdelay)

add_subdirectory(JUCE)
add_subdirectory(src)
add_subdirectory(xsimd)

include(FetchContent)
# json library
FetchContent_Declare(json 
URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
DOWNLOAD_EXTRACT_TIMESTAMP TRUE)
FetchContent_MakeAvailable(json)