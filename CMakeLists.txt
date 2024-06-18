cmake_minimum_required(VERSION 3.5)

project(neurottic)

set(NEUROTTIC_SOURCES
	${PROJECT_SOURCE_DIR}/source/app.c
	${PROJECT_SOURCE_DIR}/source/main.c
	${PROJECT_SOURCE_DIR}/source/math.c
	${PROJECT_SOURCE_DIR}/source/wad.c
)

add_executable(${PROJECT_NAME} ${NEUROTTIC_SOURCES})

include(FindPkgConfig)
pkg_search_module(SDL3 REQUIRED sdl3)
pkg_search_module(GLESV2 REQUIRED glesv2)
pkg_search_module(CGLM REQUIRED cglm)

target_include_directories(${PROJECT_NAME} PRIVATE ${SDL3_INCLUDE_DIRS} ${GLESV2_INCLUDE_DIRS} ${CGLM_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE ${SDL3_LIBRARIES} ${GLESV2_LIBRARIES} ${CGLM_LIBRARIES})

# copy assets
add_custom_target(
	${PROJECT_NAME}_copy_assets
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/data ${PROJECT_BINARY_DIR}/data
)
add_dependencies(${PROJECT_NAME} ${PROJECT_NAME}_copy_assets)
