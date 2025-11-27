# ----------------------------------------
# Build Person 1 (core engine)
# ----------------------------------------
add_subdirectory(core)

# ----------------------------------------
# Build Person 2 (simulation engine)
# ----------------------------------------
add_subdirectory(simulation)

# ----------------------------------------
# Build tests (IntegrationTest etc.)
# ----------------------------------------
add_subdirectory(tests)

# ----------------------------------------
# Optional demo main.cpp build
# ----------------------------------------
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp")
    add_executable(SmartCityCore main.cpp)
    target_link_libraries(SmartCityCore
        core_module
        simulation_module
    )
endif()
