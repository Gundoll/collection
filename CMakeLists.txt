cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(collection VERSION 1.0.0 LANGUAGES C)
set(PROEJCT_DESCRIPTION "Data structure & algorithm collections written in C")

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif()

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -fuse-ld=gold -fsanitize=address -fsanitize=leak -fsanitize=undefined -fno-sanitize=alignment -static-libasan")
endif()

# Build settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")

# Integrate with Conan if possible
if(EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
	include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
	conan_basic_setup()
endif()

# Platform Detection
add_definitions(-DARCH_WORDSIZE=${CMAKE_SIZEOF_VOID_P})

# Build settings
include_directories("${PROJECT_SOURCE_DIR}")
set(sources
	base.c queue.c
	)

add_library(objs OBJECT ${sources})
set_property(TARGET objs PROPERTY POSITION_INDEPENDENT_CODE 1)

add_library(collection $<TARGET_OBJECTS:objs>)

# CPack receipe
FILE(GLOB headerfiles *.h)
install(TARGETS collection ARCHIVE DESTINATION lib LIBRARY DESTINATION lib COMPONENT libraries) # /usr/lib
install(FILES ${headerfiles} DESTINATION include/collection COMPONENT headers)	                # /usr/include/collection
set(CPACK_COMPONENTS_ALL libraries headers)
set(CPACK_PACKAGE_VENDOR "Gundoll")
set(CPACK_PACKAGE_CONTACT "Gundoll <rnswo42b@gmail.com>")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${PROEJCT_DESCRIPTION}")
set(CPACK_GENERATOR "${CPACK_GENERATOR};STGZ;DEB;TGZ")
include(CPack)
