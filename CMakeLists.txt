cmake_minimum_required(VERSION 3.20)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "C:/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
endif()
if(NOT DEFINED VCPKG_TARGET_TRIPLET)
    set(VCPKG_TARGET_TRIPLET "x64-mingw-static" CACHE STRING "")
endif()

project(Telegram_bot LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_definitions(CPR_STATIC=1 CURL_STATICLIB=1)

find_package(CURL REQUIRED)
find_package(cpr CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

add_executable(my_tg_bot
    src/main.cpp
    src/BotClient.cpp
    src/TelegramBot.cpp
    src/WhisperAPI.cpp
    src/CommandHandler.cpp
    src/TextAnalyzer.cpp
)

target_include_directories(my_tg_bot PRIVATE include)

target_compile_definitions(my_tg_bot PRIVATE CPR_STATIC=1 CURL_STATICLIB=1)

target_link_libraries(my_tg_bot PRIVATE
    cpr::cpr
    CURL::libcurl
    nlohmann_json::nlohmann_json
    ws2_32
    wldap32
    crypt32
    advapi32
    normaliz
)