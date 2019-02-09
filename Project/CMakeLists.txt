cmake_minimum_required(VERSION 3.5)
project(lora)

message($ENV{USER})
message(${CMAKE_CURRENT_SOURCE_DIR})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/database/ ${CMAKE_CURRENT_SOURCE_DIR}/lora_support/ ${CMAKE_CURRENT_SOURCE_DIR}/packet/)

# libraries
add_library(lora_db SHARED ${CMAKE_CURRENT_SOURCE_DIR}/database/lora_db.cpp)
target_link_libraries(lora_db -lmysqlcppconn)
message("Processing MySQL library dependencies ...")

add_library(packet SHARED ${CMAKE_CURRENT_SOURCE_DIR}/packet/packet.cpp)
message("Processing packet library ...")

if($ENV{USER} STREQUAL "pi")

    message("Processing LoRa support ...")
    add_library(lora_support SHARED ${CMAKE_CURRENT_SOURCE_DIR}/lora_support/lora_support.cpp)
    target_link_libraries(lora_support -lwiringPi)

    add_executable(lora ${CMAKE_CURRENT_SOURCE_DIR}/gateway/app.cpp)
    target_link_libraries(lora -lpacket -llora_db -llora_support -lwiringPi -lmysqlcppconn)

#else

    #message("Skipping final application")

endif()
