cmake_minimum_required(VERSION 3.20)

project(charm LANGUAGES C)
set(CMAKE_C_STANDARD 11)

add_executable(${PROJECT_NAME}
    src/main.c

    src/core/beard_lib.h            src/core/beard_lib.c

    src/core/memory.h               src/core/memory.c
    src/core/value.h                src/core/value.c
    src/core/hash_table.h           src/core/hash_table.c

    src/ast/ast.h                   src/ast/ast.c
    src/ast/lexer.h                 src/ast/lexer.c
    src/ast/parser.h                src/ast/parser.c
    src/ast/token.h

    src/interpreter/frame.h         src/interpreter/frame.c
    src/interpreter/interpreter.h   src/interpreter/interpreter.c

    src/debug/debug.h               src/debug/debug.c
)

if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /w44062 /WX)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Werror -Werror=pointer-arith)
endif()

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src)
