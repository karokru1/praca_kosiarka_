cmake_minimum_required(VERSION 3.10)

project(cutGrass)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

option(ENABLE_CODE_COVERAGE "Enable code coverage" ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        release-1.12.0
)
FetchContent_MakeAvailable(googletest)

add_subdirectory(src)
add_subdirectory(test)

enable_testing()

file(GLOB_RECURSE ALL_SOURCE_FILES
    ${CMAKE_SOURCE_DIR}/src/*.cpp
    ${CMAKE_SOURCE_DIR}/src/*.hpp
    ${CMAKE_SOURCE_DIR}/test/*.cpp
    ${CMAKE_SOURCE_DIR}/test/*.hpp
)

add_custom_target(format
    COMMAND clang-format
    -i
    -style=file
    ${ALL_SOURCE_FILES}
    COMMENT "Running clang-format on source files"
)

if(ENABLE_CODE_COVERAGE)
    if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage -O0 -g")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
        set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} --coverage")
    endif()

    find_program(LCOV_EXEC lcov)
    find_program(GENHTML_EXEC genhtml)

    if(LCOV_EXEC AND GENHTML_EXEC)
        add_custom_target(coverage
            COMMAND ${LCOV_EXEC} --capture --directory ${CMAKE_BINARY_DIR} --base-directory ${CMAKE_SOURCE_DIR} --output-file coverage.info
            COMMAND ${LCOV_EXEC} --remove coverage.info '/usr/*' --output-file coverage.info.cleaned
            COMMAND ${GENHTML_EXEC} coverage.info.cleaned --output-directory coverage_report --prefix ${CMAKE_SOURCE_DIR}
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Generating coverage report"
            VERBATIM
        )
    else()
        message(WARNING "lcov or genhtml not found! Cannot generate coverage report.")
    endif()
endif()
