cmake_minimum_required(VERSION 3.20)

project(gluon LANGUAGES C)

add_executable(gluon
    src/main.c

    src/lexer.h src/lexer.c
    src/token.h src/token.c
)
