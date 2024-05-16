cmake_minimum_required(VERSION 3.20)

project(gluon LANGUAGES C)
set(CMAKE_C_STANDARD 11)

add_executable(gluon
    src/main.c

    src/memory.h      src/memory.c

    src/ast.h         src/ast.c
    src/lexer.h       src/lexer.c
    src/parser.h      src/parser.c
    src/token.h

    src/debug.h       src/debug.c
    src/interpreter.h src/interpreter.c
)

if (MSVC)
    target_compile_options(gluon PRIVATE /W4 /w44062 /WX)
else()
    target_compile_options(gluon PRIVATE -Wall -Werror)
endif()
