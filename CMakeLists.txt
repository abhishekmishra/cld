cmake_minimum_required ( VERSION 3.9 )

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

add_executable( ${PROJECT_NAME} ${CLDCLIENT_SOURCES} )
target_include_directories(${PROJECT_NAME} PRIVATE ${YOUR_DIRECTORY})

set_property(TARGET ${PROJECT_NAME} PROPERTY C_STANDARD 11)

# find_package(json-c CONFIG REQUIRED)
# target_link_libraries(${PROJECT_NAME} PUBLIC json-c::json-c)

# find_package(CURL CONFIG REQUIRED)
# target_link_libraries(${PROJECT_NAME} PUBLIC CURL::libcurl)

set(CMAKE_FIND_USE_PACKAGE_REGISTRY ON)

find_package(clibdocker CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC clibdocker::clibdocker)

find_package(coll CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC coll::coll)

find_package(json-c CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC json-c::json-c)

find_package(CURL CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC CURL::libcurl)

# To find and use linenoise, as the vcpkg build does not have cmake config
find_path(LINENOISE_INCLUDE_DIR linenoise.h)
find_library(LINENOISE_LIBRARY linenoise)
include_directories(${LINENOISE_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} PUBLIC ${LINENOISE_LIBRARY})

# To find and use linenoise, as the vcpkg build does not have cmake config
find_path(GETOPT_INCLUDE_DIR getopt.h)
find_library(GETOPT_LIBRARY getopt)
include_directories(${GETOPT_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} PUBLIC ${GETOPT_LIBRARY})
