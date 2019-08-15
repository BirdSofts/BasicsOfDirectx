# *******************************************************************************************
#/ <summary>
#/ 
#/ </summary>
#/ <created>ʆϒʅ,19.07.2019</created>
#/ <changed>ʆϒʅ,14.08.2019</changed>
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
          Core.h
          Direct2D.h
          Direct3D.h
          Game.h
          Shared.h
          Timer.h
          Utilities.h
          Window.h
)

list(APPEND SOURCE_FILES
          LearningDirectX.cpp
          Core.cpp
          Direct2D.cpp
          Direct3D.cpp
          Game.cpp
          Shared.cpp
          Timer.cpp
          Utilities.cpp
          Window.cpp
)

set(INCLUDE_DIRECTORY C:/Lua/x64/include)
set(LIBRAY_DIRECTORY C:/Lua/x64)

if(WIN32)
          list(APPEND HEADER_FILES ${INCLUDE_DIRECTORY}/sol.hpp)
          list(APPEND SOURCE_FILES ${HEADER_FILES})
endif()

# Add source to this project's executable.
add_executable(${TARGET_NAME} ${SOURCE_FILES} ${RESOURCES})

target_link_libraries(${TARGET_NAME}
          PUBLIC ${LIBRAY_DIRECTORY}/liblua53.a
)

target_include_directories(${TARGET_NAME} BEFORE
          PUBLIC ${INCLUDE_DIRECTORY}
)

add_custom_command(TARGET ${TARGET_NAME} PRE_BUILD
          COMMAND cmake -E copy_if_different
          ${LIBRAY_DIRECTORY}/lua53.dll
          $<TARGET_FILE_DIR:${TARGET_NAME}>
)

# TODO: Add tests and install targets if needed.
