cmake_minimum_required(VERSION 3.9)
project(lora)

set(PROJECT_SOURCE_DIR "/home/naveen/Documents/lora-network/Project")

# libraries
add_library(lora_db SHARED ${PROJECT_SOURCE_DIR}/database/lora_db.cpp)

# build libraries
target_link_libraries(lora_db -lmysqlcppconn)


#add_executable(lora ${PROJECT_SOURCE_DIR}/database/lora_db.cpp)
#target_link_libraries(lora lmysqlcppconn)