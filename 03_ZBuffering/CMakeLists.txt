cmake_minimum_required(VERSION 3.8.0)

project(03_ZBuffering)
set(TARGET "03_ZBuffering")

set(INSTALL_PATH "${PROJECT_SOURCE_DIR}/bin")
set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
# add source Dir path
add_definitions(-DDIR_PATH="${CMAKE_CURRENT_SOURCE_DIR}/")

include_directories("${PROJECT_SOURCE_DIR}/utils")

file(GLOB UTILS_LIST "./utils/*.*")
source_group("utils" FILES ${UTILS_LIST})

file(GLOB SRC_LIST "./*.cpp" "main.cpp")
source_group("src" FILES ${SRC_LIST})

set(ALL_SOURCES
${UTILS_LIST}
${SRC_LIST}
)

add_executable(${TARGET} ${ALL_SOURCES})

set_target_properties(${TARGET} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG "${INSTALL_PATH}")
set_target_properties(${TARGET} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE "${INSTALL_PATH}")
# SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY  ${INSTALL_PATH})

# 设置MSVC的启动项，否则Cmake生成的工程默认启动的工程永远是ALL_BUILD
if (MSVC)
     # Set the startup project .
    if (NOT ${CMAKE_VERSION} VERSION_LESS "3.6.0")
          set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
    endif ()
endif (MSVC)

# TARGET_LINK_LIBRARIES(${TARGET} "${PROJECT_SOURCE_DIR}/protoc/libprotobufd.lib")