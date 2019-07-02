cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

project(${CMAKE_PROJECT_NAME}-test CXX)

add_executable(${PROJECT_NAME}
    ${PROJECT_SOURCE_DIR}/parse/array.cpp
    ${PROJECT_SOURCE_DIR}/parse/decimal.cpp
    ${PROJECT_SOURCE_DIR}/parse/integer.cpp
    ${PROJECT_SOURCE_DIR}/parse/literals.cpp
    ${PROJECT_SOURCE_DIR}/parse/object.cpp
    ${PROJECT_SOURCE_DIR}/parse/string.cpp

    #${PROJECT_SOURCE_DIR}/write/array.cpp
    #${PROJECT_SOURCE_DIR}/write/decimal.cpp
    #${PROJECT_SOURCE_DIR}/write/integer.cpp
    ${PROJECT_SOURCE_DIR}/write/literals.cpp
    #${PROJECT_SOURCE_DIR}/write/object.cpp
    ${PROJECT_SOURCE_DIR}/write/string.cpp

    ${PROJECT_SOURCE_DIR}/compile.cpp
    ${PROJECT_SOURCE_DIR}/value.cpp
    ${PROJECT_SOURCE_DIR}/main.cpp
)

target_link_libraries(${PROJECT_NAME} ${MISSIO_JSON_STATIC_LIBRARY})

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W3 /WX /UTF-8)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Werror -pedantic)
endif()

target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR})

function(add_test_suite NAME)
    add_test(NAME ${NAME} COMMAND ${PROJECT_NAME} --test-suite=${NAME})
endfunction()

add_test_suite(parse)
add_test_suite(write)
add_test_suite(value)

add_custom_command(
    COMMENT "Run unit tests"
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_CTEST_COMMAND} --build-config $<CONFIGURATION> --output-on-failure
)
