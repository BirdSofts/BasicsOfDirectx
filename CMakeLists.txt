# *******************************************************************************************
#/ <summary>
#/ 
#/ </summary>
#/ <created>ʆϒʅ,19.07.2019</created>
#/ <changed>ʆϒʅ,08.08.2019</changed>
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
          LearningDirectX.cpp
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

if(WIN32)
          list(APPEND SOURCE_FILES ${HEADER_FILES})
endif()

# Add source to this project's executable.
add_executable(${TARGET_NAME} ${SOURCE_FILES} ${RESOURCES})

# TODO: Add tests and install targets if needed.
