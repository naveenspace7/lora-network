cmake_minimum_required(VERSION 3.5)
project(lora)

message($ENV{USER})
message(${CMAKE_CURRENT_SOURCE_DIR})

# libraries
add_library(lora_db SHARED ${CMAKE_CURRENT_SOURCE_DIR}/database/lora_db.cpp)
target_link_libraries(lora_db -lmysqlcppconn)
message("Built MySQL library dependencies ...")

add_library(packet SHARED ${CMAKE_CURRENT_SOURCE_DIR}/packet/packet.cpp)
message("Built packet library ...")

if($ENV{USER} STREQUAL "pi")
    message("Building wiring pi dependencies ...")
    add_library(lora_support SHARED ${CMAKE_CURRENT_SOURCE_DIR}/lora_support/lora_support.cpp)
    target_link_libraries(lora_support -lwiringPi)
    add_executable(lora ${CMAKE_CURRENT_SOURCE_DIR}/gateway/app.cpp)
    target_link_libraries(lora -lpacket -llora_db -llora_support -lwiringPi -lmysqlcppconn )
endif()
