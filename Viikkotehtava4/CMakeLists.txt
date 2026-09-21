cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(valot)

target_sources(app PRIVATE
    src/main.c
    src/button.c
    src/uart.c
    src/dispatcher.c
    src/debug.c
)