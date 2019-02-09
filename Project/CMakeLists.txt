cmake_minimum_required(VERSION 3.5)
project(lora)

message($ENV{USER})
message(${CMAKE_CURRENT_SOURCE_DIR})

# libraries
add_library(lora_db SHARED ${CMAKE_CURRENT_SOURCE_DIR}/database/lora_db.cpp)
target_link_libraries(lora_db -lmysqlcppconn)
message("Built MySQL library dependencies ...")

if($ENV{USER} STREQUAL "pi")
    message("Building wiring pi dependencies ...")
    #add_library(wpi SHARED ${CMAKE_CURRENT_SOURCE_DIR}/wiringpi/wpi.cpp)
    #target_link_libraries(wpi -lwiringPi)
endif()

#add_executable(lora ${CMAKE_CURRENT_SOURCE_DIR}/database/lora_db.cpp)
#target_link_libraries(lora lmysqlcppconn)