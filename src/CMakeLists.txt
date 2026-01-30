add_library(userData STATIC
    UserExpectation.cpp
)

target_include_directories(userData PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_executable(cutGrass 
    main.cpp
    Sensors.cpp
    LightSensor.cpp
    TemperatureSensor.cpp
    HumiditySensor.cpp
    PressureSensor.cpp
    RainSensor.cpp
    SoilMoistureSensor.cpp
    Decision.cpp
    Logger.cpp
    bme280Interface.cpp
    bme280.cpp
    bh1750.cpp
    mcp3008.cpp
    Camera.cpp
)

target_include_directories(cutGrass PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(cutGrass PRIVATE userData)

find_library(SQLITE3_LIB sqlite3 REQUIRED)
target_link_libraries(cutGrass PRIVATE ${SQLITE3_LIB})

find_package(OpenCV REQUIRED)
target_include_directories(cutGrass PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(cutGrass PRIVATE ${OpenCV_LIBS})

if(ENABLE_CODE_COVERAGE)
    target_compile_options(userData PRIVATE --coverage)
    target_link_libraries(cutGrass PRIVATE --coverage)
    target_link_libraries(userData PRIVATE --coverage)
endif()
