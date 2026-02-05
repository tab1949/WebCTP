
cmake_minimum_required(VERSION 3.10)
project(WebCTP)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(ICU REQUIRED COMPONENTS uc data)
find_package(unofficial-uwebsockets CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

find_library(CTP_MD_LIB thostmduserapi_se HINTS ./build/ctp /usr/local/lib /usr/lib)
find_library(CTP_TRADER_LIB thosttraderapi_se HINTS ./build/ctp /usr/local/lib /usr/lib)
find_path(CTP_HEADERS NAMES ThostFtdcTraderApi.h ThostFtdcMdApi.h PATHS ./build/ctp /usr/local/lib /usr/lib)

# find_library(US_LIB uSockets HINTS /usr/local/lib /usr/lib)
# find_library(UV_LIB uv)
# find_library(Z_LIB z)
# find_library(ICU_LIB icuuc icudata)

include_directories(${CTP_HEADERS})

add_executable(webctp 
    src/main.cpp
    src/WebSocketApp.cpp
    src/Encoding.cpp
    src/MessageHandler.cpp
    src/MarketData/Handler.cpp
    src/Trade/Handler.cpp
    src/Trade/Operations.cpp
)

# target_include_directories(webctp PRIVATE /usr/local/include)

target_link_libraries(webctp 
    # ${US_LIB}
    # ${UV_LIB}
    # ${Z_LIB}
    # ${ICU_LIB}
    # pthread
    # dl
    ICU::data
    ICU::uc
    unofficial::uwebsockets::uwebsockets
    nlohmann_json::nlohmann_json
    ${CTP_MD_LIB}
    ${CTP_TRADER_LIB}
)

# add_executable(logger_test
#     test/log.cpp
# )
# target_include_directories(logger_test PRIVATE src)
# target_link_libraries(logger_test pthread)