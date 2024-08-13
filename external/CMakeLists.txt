# Copyright (c) Mor

set(CMAKE_COMPILE_WARNING_AS_ERROR OFF)

set(GRAPHICS "GRAPHICS_API_OPENGL_43")
set(USE_EXTERNAL_GLFW ON)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
  add_compile_options( -Wno-incompatible-pointer-types -Wno-int-to-void-pointer-cast)
endif()

add_subdirectory(raylib EXCLUDE_FROM_ALL SYSTEM)
