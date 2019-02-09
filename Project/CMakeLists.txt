cmake_minimum_required(VERSION 3.9)
project(lora)
set(PROJECT_SOURCE_DIR "/home/naveen/Documents/lora-network/Project")
#message(${PROJECT_SOURCE_DIR})
message($ENV{USER})

# libraries
add_library(lora_db SHARED ${PROJECT_SOURCE_DIR}/database/lora_db.cpp)
target_link_libraries(lora_db -lmysqlcppconn)
message("Built MySQL library dependencies ...")

if($ENV{USER} STREQUAL "pi")
    message("Building wiring pi dependencies ...")
    #add_library(wpi SHARED ${PROJECT_SOURCE_DIR}/wiringpi/wpi.cpp)
    #target_link_libraries(wpi -lwiringPi)
endif()

#add_executable(lora ${PROJECT_SOURCE_DIR}/database/lora_db.cpp)
#target_link_libraries(lora lmysqlcppconn)