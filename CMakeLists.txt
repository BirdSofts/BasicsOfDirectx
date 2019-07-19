# *******************************************************************************************
#/ <summary>
#/ 
#/ </summary>
#/ <created>ʆϒʅ,19.07.2019</created>
#/ <changed>ʆϒʅ,20.07.2019</changed>
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
          LearningDirectX.h
          Window.h
          DirectX.h
)

list(APPEND HEADER_FILES
          LearningDirectX.cpp
          Window.cpp
          DirectX.cpp
)

if(WIN32)
          list(APPEND SOURCE_FILES ${HEADER_FILES})
endif()

# Add source to this project's executable.
add_executable(${TARGET_NAME} ${SOURCE_FILES} ${RESOURCES})

# TODO: Add tests and install targets if needed.
