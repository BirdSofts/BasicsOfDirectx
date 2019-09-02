# *******************************************************************************************
#/ <summary>
#/ 
#/ </summary>
#/ <created>ʆϒʅ,19.07.2019</created>
#/ <changed>ʆϒʅ,03.09.2019</changed>
# *******************************************************************************************

# CMakeList.txt : CMake project for LearningDirectX, include source and define
# project specific logic here.
#
cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

set(TARGET_NAME LearningDirectX)

set(PROJECT_NAME ${TARGET_NAME})

project(${PROJECT_NAME}
          VERSION 1.0.0.0
          DESCRIPTION "Learning DirectX Notes"
          HOMEPAGE_URL "https://github.com/BirdSofts/LearningDirectX"
          LANGUAGES CXX
)

list(APPEND RESOURCES
          ${TARGET_NAME}.rc
          resource.h
)

list(APPEND HEADER_FILES
          2Dmodels.h
          Camera.h
          Core.h
          Direct2D.h
          Direct3D.h
          Game.h
          Light.h
          Shader.h
          Shared.h
          Texture.h
          Timer.h
          Utilities.h
          Window.h
)

list(APPEND SOURCE_FILES
          LearningDirectX.cpp
          2Dmodels.cpp
          Camera.cpp
          Core.cpp
          Direct2D.cpp
          Direct3D.cpp
          Game.cpp
          Light.cpp
          Shader.cpp
          Shared.cpp
          Texture.cpp
          Timer.cpp
          Utilities.cpp
          Window.cpp
)

if(WIN32)
          list(APPEND SOURCE_FILES ${HEADER_FILES})
endif()

# Lua and Sol third party library inclusion and linkage
set(LUA_ROOT_DIR "C:/Lua/x64")
if(NOT LUA_ROOT_DIR)
message(FATAL_ERROR "Lua root path couldn't be found!")
endif()
set(INCLUDE_DIRECTORY ${LUA_ROOT_DIR}/include)
set(LIBRARY_DIRECTORY ${LUA_ROOT_DIR})
message(${INCLUDE_DIRECTORY})
message(${LIBRARY_DIRECTORY})

find_file(LUA_LIBRARY liblua53.a HINTS ${LIBRARY_DIRECTORY} PATH_SUFFIXES lib)
if (NOT LUA_LIBRARY)
message(FATAL_ERROR "liblua53.a couldn't be found!")
endif()

# Add source to this project's executable.
add_executable(${TARGET_NAME} ${SOURCE_FILES} ${RESOURCES})

target_include_directories(${TARGET_NAME} BEFORE
          PUBLIC ${INCLUDE_DIRECTORY}
)

target_link_libraries(${TARGET_NAME}
          PUBLIC ${LUA_LIBRARY}
)

set_target_properties(${TARGET_NAME} PROPERTIES
          LINK_FLAGS /SUBSYSTEM:WINDOWS
)

add_custom_command(TARGET ${TARGET_NAME} PRE_BUILD
          COMMAND cmake -E copy_if_different
          ${LUA_ROOT_DIR}/lua53.dll
          $<TARGET_FILE_DIR:${TARGET_NAME}>
)

# TODO: Add tests and install targets if needed.
