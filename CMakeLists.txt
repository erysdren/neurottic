cmake_minimum_required(VERSION 3.5)

project(neurottic)

add_executable(${PROJECT_NAME}
	app.c main.c math.c
)

include(FindPkgConfig)
pkg_search_module(SDL3 REQUIRED sdl3)

target_include_directories(${PROJECT_NAME} PRIVATE ${SDL3_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE ${SDL3_LIBRARIES})

# copy assets
add_custom_target(
	${PROJECT_NAME}_copy_assets
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/data ${PROJECT_BINARY_DIR}/data
)
add_dependencies(${PROJECT_NAME} ${PROJECT_NAME}_copy_assets)
