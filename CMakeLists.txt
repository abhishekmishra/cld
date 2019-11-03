cmake_minimum_required ( VERSION 3.9 )

include(CheckIncludeFile)

project ( cldclient C )

# Set the project source dir (just convention)
set( CLDCLIENT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR} )
set( CLDCLIENT_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR} )

# It's useful to setup the current version of our code in the build system
# using a `semver` style
set ( CLDCLIENT_VERSION_MAJOR 1 )
set ( CLDCLIENT_VERSION_MINOR 0 )
set ( CLDCLIENT_VERSION_PATCH 0 )

# Include Directories
# In GCC, this will invoke the "-I" command
include_directories( src )

# Lists
# Setup the list of source files
set( CLDCLIENT_SOURCES
  src/cld_command.c
  src/cld_common.c
  src/cld_ctr.c
  src/cld_dict.c
  src/cld_img.c
  src/cld_net.c
  src/cld_progress.c
  src/cld_sys.c
  src/cld_table.c
  src/cld_vol.c
  src/cld.c
  src/tokenizer.c

  src/cld_command.h
  src/cld_common.h
  src/cld_ctr.h
  src/cld_dict.h
  src/cld_img.h
  src/cld_net.h
  src/cld_progress.h
  src/cld_sys.h
  src/cld_table.h
  src/cld_vol.h
  src/histedit.h
)

if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux")
	message(STATUS ">>> Linux, using pkg-config since vcpkg is broken")
	find_package(PkgConfig REQUIRED)
	pkg_check_modules(json-c REQUIRED IMPORTED_TARGET json-c)
	pkg_check_modules(curl REQUIRED IMPORTED_TARGET libcurl)
	pkg_check_modules(archive REQUIRED IMPORTED_TARGET libarchive)
endif()

add_executable( ${PROJECT_NAME} ${CLDCLIENT_SOURCES} )
target_include_directories(${PROJECT_NAME} PRIVATE ${YOUR_DIRECTORY})

set_property(TARGET ${PROJECT_NAME} PROPERTY C_STANDARD 11)

if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux")
	target_link_libraries(${PROJECT_NAME} PUBLIC json-c)
	target_link_libraries(${PROJECT_NAME} PUBLIC curl)
	target_link_libraries(${PROJECT_NAME} PUBLIC archive)
else()
	find_package(json-c CONFIG REQUIRED)
	target_link_libraries(${PROJECT_NAME} PUBLIC json-c::json-c)

	find_package(CURL CONFIG REQUIRED)
	target_link_libraries(${PROJECT_NAME} PUBLIC CURL::libcurl)
endif()

set(CMAKE_FIND_USE_PACKAGE_REGISTRY ON)

find_package(coll CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC coll::coll)

find_package(clibdocker CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC clibdocker::clibdocker)

check_include_file("getopt.h" HAVE_GETOPT)

if(NOT HAVE_GETOPT)
	# To find and use linenoise, as the vcpkg build does not have cmake config
	find_path(GETOPT_INCLUDE_DIR getopt.h)
	find_library(GETOPT_LIBRARY getopt)
	include_directories(${GETOPT_INCLUDE_DIR})
	target_link_libraries(${PROJECT_NAME} PUBLIC ${GETOPT_LIBRARY})
endif (NOT HAVE_GETOPT)
