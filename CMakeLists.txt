
cmake_minimum_required(VERSION 3.10)
project(WebCTP)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_library(US_LIB uSockets HINTS /usr/local/lib /usr/lib)
find_library(UV_LIB uv)
find_library(Z_LIB z)
find_library(CTP_MD_LIB thostmduserapi_se HINTS /usr/local/lib /usr/lib)
find_library(CTP_TRADER_LIB thosttraderapi_se HINTS /usr/local/lib /usr/lib)
find_library(ICU_LIB icuuc icudata)

add_executable(WebCTP 
    src/main.cpp
    src/Encoding.cpp
    src/MessageHandler.cpp
    src/MarketData/Handler.cpp
    # src/MarketData/Operations.cpp
    src/Trade/Handler.cpp
    src/Trade/Operations.cpp
)

target_include_directories(WebCTP PRIVATE /usr/local/include)

target_link_libraries(WebCTP 
    ${US_LIB}
    ${UV_LIB}
    ${Z_LIB}
    ${CTP_MD_LIB}
    ${CTP_TRADER_LIB}
    ${ICU_LIB}
    pthread
    dl
)
