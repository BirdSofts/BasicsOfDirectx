# ============================================================================
# / <summary>
# / CMakelists.txt
# / DirectXIntroduction
# / created by Mehrdad Soleimanimajd on 04.07.2023
# / </summary>
# / <created>ʆϒʅ, 04.07.2023</created>
# / <changed>ʆϒʅ, 04.07.2023</changed>
# ============================================================================

cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

set(TARGET_NAME DirectXIntroduction)

set(CMAKE_CXX_STANDARD_REQUIRED on)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_TOOLCHAIN_FILE "C:/src/vcpkg/scripts/buildsystems/vcpkg.cmake")

set(PROJECT_NAME ${TARGET_NAME})
project(${PROJECT_NAME}
  VERSION 1.0.1.0
  DESCRIPTION "DirectX Introduction"
  HOMEPAGE_URL "https://github.com/s-mehrdad/DirectXIntroduction"
  LANGUAGES CXX
)

set(VerCompanyName "BirdSofts")
set(VerFileDescription "DirectX Introduction")
set(VerFileVersion "1,0,1,0")
set(VerInternalName ${TARGET_NAME})
set(VerLegalCopyright "Copyright (C) 2019-2023")
set(VerLegalTrademarks1 "BS")
set(VerLegalTrademarks2 "BSs")
set(VerOriginalFilename ${TARGET_NAME})
set(VerProductName ${TARGET_NAME})
set(VerProductVersion ${CMAKE_PROJECT_VERSION})

configure_file(
  DirectXIntroduction.rc.in
  DirectXIntroduction.rc
  @ONLY
  NEWLINE_STYLE LF
)

# Note: ISSUE: lua and sol libraries linkage
find_package(lua REQUIRED)
find_package(sol CONFIG REQUIRED)

list(APPEND RESOURCES
  ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}.rc
  resource.h
)

list(APPEND HEADER_FILES
  DirectXIntroduction.h
  Camera.h
  Core.h
  Direct2D.h
  Direct3D.h
  Game.h
  Light.h
  ModelFormats.h
  Polygons.h
  Shader.h
  Shared.h
  Texture.h
  Timer.h
  Universe.h
  Utilities.h
  Window.h
)

list(APPEND SOURCE_FILES
  DirectXIntroduction.cpp
  Camera.cpp
  Core.cpp
  Direct2D.cpp
  Direct3D.cpp
  Game.cpp
  Light.cpp
  ModelFormats.cpp
  Polygons.cpp
  Shader.cpp
  Shared.cpp
  Texture.cpp
  Timer.cpp
  Universe.cpp
  Utilities.cpp
  Window.cpp
)

if(WIN32)
  list(APPEND SOURCE_FILES ${HEADER_FILES})
endif()

# Lua and Sol third party library inclusion and linkage
# set(LUA_ROOT_DIR "C:/Lua/x64")
# if(NOT LUA_ROOT_DIR)
# message(FATAL_ERROR "Lua root path couldn't be found!")
# endif()
# set(INCLUDE_DIRECTORY ${LUA_ROOT_DIR}/include)
# set(LIBRARY_DIRECTORY ${LUA_ROOT_DIR})
# message(${INCLUDE_DIRECTORY})
# message(${LIBRARY_DIRECTORY})

# find_file(LUA_LIBRARY liblua53.a HINTS ${LIBRARY_DIRECTORY} PATH_SUFFIXES lib)
# if (NOT LUA_LIBRARY)
# message(FATAL_ERROR "liblua53.a couldn't be found!")
# endif()
add_executable(${TARGET_NAME} ${SOURCE_FILES} ${RESOURCES})

target_link_libraries(${TARGET_NAME}

  # PUBLIC lua
  PRIVATE sol2::sol2
)

# target_include_directories(${TARGET_NAME} BEFORE
# PUBLIC ${INCLUDE_DIRECTORY}
# )

# target_link_libraries(${TARGET_NAME}
# PUBLIC ${LUA_LIBRARY}
# )
set_target_properties(${TARGET_NAME} PROPERTIES
  LINK_FLAGS /SUBSYSTEM:WINDOWS
)

# add_custom_command(TARGET ${TARGET_NAME} PRE_BUILD
# COMMAND cmake -E copy_if_different
# ${LUA_ROOT_DIR}/lua53.dll
# $<TARGET_FILE_DIR:${TARGET_NAME}>
# )
