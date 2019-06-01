cmake_minimum_required(VERSION 3.5)

set(APP_TARGET wasm_webgl2)

project(${APP_TARGET}_project)

set(BIN_DIR ${PROJECT_SOURCE_DIR}/bin)
set(ASSETS_DIR ${PROJECT_SOURCE_DIR}/assets)

include_directories(${PROJECT_SOURCE_DIR}/include)
file(GLOB SOURCE_FILES ${PROJECT_SOURCE_DIR}/src/app/*.cpp)

if (APPLE)
  set(PLATFORM_SOURCE_FILES ${PROJECT_SOURCE_DIR}/src/glad/glad.c)
endif()

if (EMSCRIPTEN)
  set(CMAKE_CXX_FLAGS "-std=c++1z -fno-exceptions -fno-rtti")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s WASM=1 -s USE_WEBGL2=1 -s ALLOW_MEMORY_GROWTH=1 -s DISABLE_EXCEPTION_CATCHING=1 -s NO_FILESYSTEM=1")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s DISABLE_DEPRECATED_FIND_EVENT_TARGET_BEHAVIOR=1")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s EXPORTED_FUNCTIONS=\"['_free']\"")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s EXTRA_EXPORTED_RUNTIME_METHODS=\"['allocateUTF8', 'UTF8ToString']\"")
  set(CMAKE_CXX_FLAGS_DEBUG "-g4 -s DEMANGLE_SUPPORT=1")
  set(CMAKE_CXX_FLAGS_RELEASE_INIT "-Oz -DNDEBUG --llvm-lto 1")

  set(BIN_DIR ${PROJECT_SOURCE_DIR}/web/dist)

  file(GLOB PLATFORM_SOURCE_FILES ${PROJECT_SOURCE_DIR}/src/emscripten/*.cpp)
endif ()

add_executable(${APP_TARGET} ${SOURCE_FILES} ${PLATFORM_SOURCE_FILES})
set_target_properties(${APP_TARGET} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR})

add_custom_target(inline_shaders
    COMMAND python ${PROJECT_SOURCE_DIR}/generate_inline_shaders.py ${PROJECT_SOURCE_DIR})
add_dependencies(${APP_TARGET} inline_shaders)

# if (EMSCRIPTEN)
#   add_custom_command(TARGET ${APP_TARGET} POST_BUILD
#       COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/emscripten/index.html ${BIN_DIR} DEPENDS ${BIN_DIR})
# endif ()
