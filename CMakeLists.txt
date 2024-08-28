cmake_minimum_required(VERSION 3.5)

project(NEUROTTIC LANGUAGES C VERSION 0.0.1)

set(NEUROTIC_EXEC neurottic)
set(NEUROTTIC_SOURCES
	${PROJECT_SOURCE_DIR}/source/audio_manager.c
	${PROJECT_SOURCE_DIR}/source/console.c
	${PROJECT_SOURCE_DIR}/source/darkwar.c
	${PROJECT_SOURCE_DIR}/source/filesystem.c
	${PROJECT_SOURCE_DIR}/source/logging.c
	${PROJECT_SOURCE_DIR}/source/lump_manager.c
	${PROJECT_SOURCE_DIR}/source/main.c
	${PROJECT_SOURCE_DIR}/source/mapset.c
	${PROJECT_SOURCE_DIR}/source/math_utilities.c
	${PROJECT_SOURCE_DIR}/source/renderer.c
	${PROJECT_SOURCE_DIR}/source/renderer_software.c
	${PROJECT_SOURCE_DIR}/source/rlew.c
)

add_executable(${NEUROTIC_EXEC} ${NEUROTTIC_SOURCES})
set_property(TARGET ${NEUROTIC_EXEC} PROPERTY C_STANDARD 99)

# target_compile_options(${NEUROTIC_EXEC} PUBLIC -fsanitize=address)
# target_link_options(${NEUROTIC_EXEC} PUBLIC -fsanitize=address)

include(FindPkgConfig)
pkg_search_module(SDL3 REQUIRED sdl3)
pkg_search_module(SDL3_MIXER REQUIRED sdl3-mixer)
pkg_search_module(SDL3_NET REQUIRED sdl3-net)
pkg_search_module(GLESV2 REQUIRED glesv2)
pkg_search_module(CGLM REQUIRED cglm)

target_include_directories(${NEUROTIC_EXEC} PRIVATE
	${SDL3_INCLUDE_DIRS} ${SDL3_MIXER_INCLUDE_DIRS} ${SDL3_NET_INCLUDE_DIRS}
	${GLESV2_INCLUDE_DIRS} ${CGLM_INCLUDE_DIRS}
)

target_link_libraries(${NEUROTIC_EXEC} PRIVATE
	${SDL3_LIBRARIES} ${SDL3_MIXER_LIBRARIES} ${SDL3_NET_LIBRARIES}
	${GLESV2_LIBRARIES} ${CGLM_LIBRARIES}
)
