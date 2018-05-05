cmake_minimum_required(VERSION 3.5)

set(APP_NAME wasm_webgl2)

project(${APP_NAME}_project)

set(BIN_DIR ${PROJECT_SOURCE_DIR}/bin)
set(ASSETS_DIR ${PROJECT_SOURCE_DIR}/assets)


include_directories(${PROJECT_SOURCE_DIR}/include)

file(GLOB SOURCE_FILES ${PROJECT_SOURCE_DIR}/src/app/*.cpp)

if (APPLE)
  set(PLATFORM_SOURCE_FILES ${PROJECT_SOURCE_DIR}/src/glad/glad.c)
endif()

if (EMSCRIPTEN)
  set(CMAKE_CXX_FLAGS "-std=c++1z -fno-exceptions -fno-rtti -s WASM=1 -s USE_WEBGL2=1 -s ALLOW_MEMORY_GROWTH=0 -s DISABLE_EXCEPTION_CATCHING=1 -s NO_FILESYSTEM=1 -s EXPORTED_FUNCTIONS=\"['_free']\" -s EXTRA_EXPORTED_RUNTIME_METHODS=\"['allocateUTF8']\"")
  set(CMAKE_CXX_FLAGS_DEBUG "-g4 -s DEMANGLE_SUPPORT=1")
  set(CMAKE_CXX_FLAGS_RELEASE_INIT "-Oz -DNDEBUG --llvm-lto 1")

  set(BIN_DIR ${BIN_DIR}/wasm)

  file(GLOB PLATFORM_SOURCE_FILES ${PROJECT_SOURCE_DIR}/src/emscripten/*.cpp)
endif ()

add_executable(${APP_NAME} ${SOURCE_FILES} ${PLATFORM_SOURCE_FILES})
set_target_properties(${APP_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR})

add_custom_target(inline_shaders
    COMMAND python ${PROJECT_SOURCE_DIR}/generate_inline_shaders.py ${PROJECT_SOURCE_DIR})
add_dependencies(${APP_NAME} inline_shaders)

if (EMSCRIPTEN)
  set(BUILD_ASSETS_DIR ${CMAKE_BINARY_DIR}/assets)
  # add_custom_command(TARGET ${APP_NAME} PRE_BUILD
  #     COMMAND ${CMAKE_COMMAND} -E create_symlink ${ASSETS_DIR} ${BUILD_ASSETS_DIR} DEPENDS ${BUILD_ASSETS_DIR})
  add_custom_command(TARGET ${APP_NAME} POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/emscripten/index.html ${BIN_DIR} DEPENDS ${BIN_DIR})
endif ()
