cmake_minimum_required(VERSION 3.24)
project(Parser_json2csv C)

set(CMAKE_C_STANDARD 11)

add_executable(Parser_json2csv main.c cJSON.h cJSON.c)
