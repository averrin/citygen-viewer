cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS true)
set(CMAKE_BUILD_TYPE Release)
set(OpenGL_GL_PREFERENCE "GLVND")

project(citygen-viewer)

set(PROJECT_PATH "${CMAKE_CURRENT_SOURCE_DIR}")
set(EXE_NAME ${PROJECT_NAME} )

add_subdirectory(backward-cpp)

set(IMGUI_INCLUDE_DIRS ${PROJECT_PATH}/include/imgui)
file(GLOB IMGUI_SOURCES ${PROJECT_PATH}/include/imgui/*.cpp)
include_directories("${IMGUI_INCLUDE_DIRS}")
set(IMGUI_SFML_INCLUDE_DIRS ${PROJECT_PATH}/include/imgui-sfml)
file(GLOB IMGUI_SFML_SOURCES ${PROJECT_PATH}/include/imgui-sfml/*.cpp)
add_definitions(-DIMGUI_USER_CONFIG="${IMGUI_SFML_INCLUDE_DIRS}/imconfig-SFML.h")
include_directories("${IMGUI_SFML_INCLUDE_DIRS}")

add_definitions(-D_USE_MATH_DEFINES)
include_directories("${PROJECT_PATH}/include")

find_package(fmt CONFIG REQUIRED)

set(SFML_STATIC_LIBRARIES FALSE)
find_package(SFML 2.5 COMPONENTS REQUIRED system window graphics)

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})
include_directories(${SFML_INCLUDE_DIR})

set(CITYGEN_PATH ${PROJECT_PATH}/include/citygen)
set(CITYGEN_INCLUDE_DIRS ${CITYGEN_PATH}/include)
include_directories(${CITYGEN_INCLUDE_DIRS})
MESSAGE(STATUS "CityGen include path: ${CITYGEN_INCLUDE_DIRS}")
file(GLOB CITYGEN_SOURCES "${CITYGEN_PATH}/src/*.cpp")

# CGAL
find_package( Boost REQUIRED )
if ( NOT Boost_FOUND )
  message(STATUS "This project requires the Boost library, and will not be compiled.")
  return()
endif()
find_package( CGAL QUIET COMPONENTS  )

if ( NOT CGAL_FOUND )
  message(STATUS "This project requires the CGAL library, and will not be compiled.")
  return()
endif()

include( ${CGAL_USE_FILE} )

# THOR
# find_package( Thor 2 REQUIRED )
find_library(THOR_LIBRARY NAMES thor PATHES /usr/local/lib)
include_directories(${THOR_INCLUDE_DIR})


file(GLOB SOURCE "src/*.cpp")
add_executable(${EXE_NAME}
  ${BACKWARD_ENABLE}
  ${IMGUI_SOURCES} ${IMGUI_SFML_SOURCES}
  ${CITYGEN_SOURCES}

  ${PROJECT_PATH}/src/drawableMap.cpp
  ${PROJECT_PATH}/src/application.cpp
  ${PROJECT_PATH}/src/main.cpp
  )
add_backward(${EXE_NAME})
add_to_cached_list( CGAL_EXECUTABLE_TARGETS citygen-viewer )


target_link_libraries(${EXE_NAME} PRIVATE
  fmt::fmt fmt::fmt-header-only
  ${CGAL_LIBRARIES} ${CGAL_3RD_PARTY_LIBRARIES}
  thor
  X11 FLAC UDev
  sfml-system sfml-window sfml-graphics OpenGL GL
  )
