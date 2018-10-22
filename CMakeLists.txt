cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_TOOLCHAIN_FILE "../vcpkg/scripts/buildsystems/vcpkg.cmake")
set(CMAKE_EXPORT_COMPILE_COMMANDS true)

project(citygen-viewer)
set(PROJECT_PATH "${CMAKE_CURRENT_SOURCE_DIR}")
set( EXE_NAME ${PROJECT_NAME} )

add_subdirectory(backward-cpp)

add_definitions(-DIMGUI_USER_CONFIG="${PROJECT_PATH}/include/imgui-sfml/imconfig-SFML.h")
add_definitions(-D_USE_MATH_DEFINES)
include_directories(
  "${PROJECT_PATH}/include"
  "${PROJECT_PATH}/include/imgui"
  "${PROJECT_PATH}/include/imgui-sfml"
  )

find_package(fmt CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(SFML 2 COMPONENTS CONFIG REQUIRED system window graphics)

file(GLOB IMLIB_SFML_SOURCE "${PROJECT_PATH}/include/imgui-sfml/*.cpp" "${PROJECT_PATH}/include/imgui-sfml/*.h")
add_library(imgui-sfml ${IMLIB_SFML_SOURCE})

set(OpenGL_GL_PREFERENCE "GLVND")
# set(OpenGL_GL_PREFERENCE "LEGACY")
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})
include_directories(${SFML_INCLUDE_DIR})
find_path(IMLIB_SOURCE imgui.h)
include_directories(${IMLIB_SOURCE})

find_package( CGAL QUIET COMPONENTS  )

if ( NOT CGAL_FOUND )

  message(STATUS "This project requires the CGAL library, and will not be compiled.")
  return()

endif()

include( ${CGAL_USE_FILE} )

find_package( Boost REQUIRED )

if ( NOT Boost_FOUND )
  message(STATUS "This project requires the Boost library, and will not be compiled.")
  return()
endif()


file(GLOB SOURCE "src/*.cpp")
add_executable(${EXE_NAME}
  ${BACKWARD_ENABLE}
  ${IMLIB_SFML_SOURCE}

  ${PROJECT_PATH}/src/application.cpp
  ${PROJECT_PATH}/src/main.cpp
  )
add_backward(${EXE_NAME})
add_to_cached_list( CGAL_EXECUTABLE_TARGETS citygen-viewer )


target_link_libraries(${EXE_NAME} PRIVATE X11 FLAC UDev OpenAL imgui::imgui imgui-sfml fmt::fmt fmt::fmt-header-only
  sfml-system sfml-window sfml-graphics OpenGL GL thor
  ${CGAL_LIBRARIES} ${CGAL_3RD_PARTY_LIBRARIES}
  )
