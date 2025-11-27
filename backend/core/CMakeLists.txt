file(GLOB CORE_SRC *.cpp)

add_library(core_module ${CORE_SRC})

target_include_directories(core_module PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
