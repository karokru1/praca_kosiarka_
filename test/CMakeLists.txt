include(GoogleTest)
include(CTest)

set(TEST
    UserExpectationTest.cpp
    DecisionTest.cpp
    LoggerTest.cpp
    SensorsTest.cpp
    ../src/Decision.cpp
    ../src/Logger.cpp
    ../src/LightSensor.cpp
    ../src/SoilMoistureSensor.cpp
    ../src/RainSensor.cpp
)

add_executable(UserExpectationTest ${TEST} doubles/bh1750.cpp doubles/mcp3008.cpp)
target_include_directories(UserExpectationTest PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/doubles ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(UserExpectationTest PRIVATE userData gtest_main)

find_library(SQLITE3_LIB sqlite3 REQUIRED)
target_link_libraries(UserExpectationTest PRIVATE ${SQLITE3_LIB})

gtest_discover_tests(UserExpectationTest)

if(ENABLE_CODE_COVERAGE)
    target_compile_options(UserExpectationTest PRIVATE --coverage)
endif()
