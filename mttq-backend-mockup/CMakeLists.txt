cmake_minimum_required(VERSION 3.10)

# 1) vcpkg toolchain must be set BEFORE project()
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE
        "C:/vcpkg/scripts/buildsystems/vcpkg.cmake"
        CACHE FILEPATH "" FORCE)
endif()

project(mqtt_printer_client)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(VCPKG_X64_ROOT "C:/vcpkg/installed/x64-windows")

set(SOURCES
    src/mqtt_client.cpp
    src/main.cpp
)

# 2) Packages from vcpkg that provide configs
find_package(nlohmann_json CONFIG REQUIRED)
find_package(OpenSSL REQUIRED)

add_executable(mqtt_client ${SOURCES})

# Your headers + vcpkg headers
target_include_directories(mqtt_client PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    "${VCPKG_X64_ROOT}/include"
)

# Where vcpkg .lib files live
target_link_directories(mqtt_client PRIVATE
    "C:/vcpkg/installed/x64-windows/lib"
)

# Link Paho C library (synchronous API)
target_link_libraries(mqtt_client
    PRIVATE
        paho-mqtt3c       # Synchronous C API (required for MQTTClient_*)
        nlohmann_json::nlohmann_json
        OpenSSL::SSL
        OpenSSL::Crypto
)

if (MSVC)
    target_compile_options(mqtt_client PRIVATE /W4)
else()
    target_compile_options(mqtt_client PRIVATE -Wall -Wextra -Wpedantic)
endif()
