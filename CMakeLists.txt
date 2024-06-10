cmake_minimum_required(VERSION 3.20)

project(charm LANGUAGES C)
set(CMAKE_C_STANDARD 11)

add_executable(${PROJECT_NAME}
    src/main.c

    src/core/memory.h               src/core/memory.c
    src/core/value.h                src/core/value.c
    src/core/cell.h                 src/core/cell.c
    src/core/dyn_array.h            src/core/dyn_array.c
    src/core/hash_table.h           src/core/hash_table.c

    src/ast/ast.h                   src/ast/ast.c
    src/ast/lexer.h                 src/ast/lexer.c
    src/ast/parser.h                src/ast/parser.c
    src/ast/token.h

    src/compiler/chunk.h            src/compiler/chunk.c
    src/compiler/compiler.h         src/compiler/compiler.c

    src/interpreter/frame.h         src/interpreter/frame.c
    src/interpreter/treewalk.h      src/interpreter/treewalk.c
    src/interpreter/vm.h            src/interpreter/vm.c

    src/debug/debug.h               src/debug/debug.c
                                    src/debug/ast_printer.c
                                    src/debug/disassembler.c
)

if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /w44062 /WX)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Werror -Werror=pointer-arith)
endif()

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src)
