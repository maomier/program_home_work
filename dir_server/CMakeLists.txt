cmake_minimum_required(VERSION 3.10)
project(ctest C)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -g -pthread")

add_executable(dir_server src/funs.c src/ref_count.c src/recv_buff.c src/cmd_str_list.c src/send_buff_list.c src/ls_cmd.c src/cd_cmd.c src/cmd.c src/send_buff.c src/send_buff_list.c src/session.c src/server.c src/main.c)