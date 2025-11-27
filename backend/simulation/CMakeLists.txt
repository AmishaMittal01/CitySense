file(GLOB SIM_SRC *.cpp)

add_library(simulation_module ${SIM_SRC})

target_include_directories(simulation_module PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(simulation_module core_module)
