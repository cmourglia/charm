cmake_minimum_required(VERSION 3.20)

project(gluon LANGUAGES C)
set(CMAKE_C_STANDARD 11)

add_executable(gluon
    src/main.c

    src/token.h src/token.c
    src/ast.h    src/ast.c
    src/lexer.h  src/lexer.c
)
