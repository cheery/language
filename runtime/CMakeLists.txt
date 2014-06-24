cmake_minimum_required (VERSION 2.6)
project (ambrosia)
include_directories(inc)
add_definitions(-O3 -Wall)
enable_language(ASM)

execute_process(
    COMMAND python ${CMAKE_SOURCE_DIR}/tools/bytecode-format.py ambrosia-bytecode.json
    INPUT_FILE ${CMAKE_SOURCE_DIR}/src/vm.c
)

SET(AMBROSIA_SRC
    src/api.c
    src/baselib.c
    src/cmp.c
    src/closure.c
    src/context.c
    src/dict.c
    src/greenlet.c
    src/list.c
    src/stack.c
    src/string.c
    src/type.c
    src/value.c
    src/vm.c
)
add_library(ambrosia SHARED ${AMBROSIA_SRC})
add_library(ambrosia_static STATIC ${AMBROSIA_SRC})

add_executable(ambrosia_bin src/main.c)
target_link_libraries(ambrosia_bin ambrosia_static)
set_target_properties(ambrosia_bin PROPERTIES OUTPUT_NAME ambrosia)
