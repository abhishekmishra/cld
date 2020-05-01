cmake_minimum_required ( VERSION 3.9 )

project ( cld C )

# Set the project source dir (just convention)
set( CLD_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR} )
set( CLD_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR} )

# It's useful to setup the current version of our code in the build system
# using a `semver` style
set ( CLD_VERSION_MAJOR 0 )
set ( CLD_VERSION_MINOR 1 )
set ( CLD_VERSION_PATCH 0 )

# Include Directories
# In GCC, this will invoke the "-I" command
include_directories( src )

# Lists
# Setup the list of source files
set( CLD_SOURCES
  src/cld_common.c
  src/cld_ctr.c
  src/cld_img.c
  src/cld_net.c
  src/cld_sys.c
  src/cld_vol.c
  src/cld.c
  src/tokenizer.c
  src/cld_lua.c
  src/mustach.c
  src/mustach-json-c.c

  src/cld_common.h
  src/cld_ctr.h
  src/cld_img.h
  src/cld_net.h
  src/cld_sys.h
  src/cld_vol.h
  src/histedit.h
  src/cld_lua.h
  src/mustach.h
  src/mustach-json-c.h
)

include(FindLua)
if (LUA_FOUND)
  find_package(lua REQUIRED)
  message("-- lua found at ${LUA_LIBRARIES} ${LUA_INCLUDE_DIR}")
  message("-- lua version is ${LUA_VERSION_STRING}")
  add_compile_definitions(LUA_ENABLED)
else ()
  message( FATAL_ERROR " -- ERROR: lua not found.")
endif (LUA_FOUND)

add_executable( ${PROJECT_NAME} ${CLD_SOURCES} )
target_include_directories(${PROJECT_NAME} PUBLIC src)

if (LUA_FOUND)
  target_include_directories(${PROJECT_NAME} PRIVATE ${LUA_INCLUDE_DIR})
  target_link_libraries(${PROJECT_NAME} PRIVATE ${LUA_LIBRARIES})
endif (LUA_FOUND)

# see mustach documentation for this
add_definitions(-DNO_OPEN_MEMSTREAM)

set_property(TARGET ${PROJECT_NAME} PROPERTY C_STANDARD 11)
set(CMAKE_FIND_USE_PACKAGE_REGISTRY ON)

find_package(json-c CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC json-c::json-c)

find_package(CURL CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC CURL::libcurl)

# To find and use libarchive, as the vcpkg build does not have cmake config
# See https://github.com/microsoft/vcpkg/issues/8839#issuecomment-558066466
# for additional lookup to ZLIB
find_package(ZLIB REQUIRED)
find_path(LIBARCHIVE_INCLUDE_DIR archive.h)
find_library(LIBARCHIVE_LIBRARY archive)
include_directories(${LIBARCHIVE_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} PUBLIC ${LIBARCHIVE_LIBRARY} ZLIB::ZLIB)

find_package(coll CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC coll::coll)

find_package(clibdocker CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC clibdocker::clibdocker)

find_package(cliutils CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC cliutils::cliutils)

include(CheckIncludeFile)
check_include_file("getopt.h" HAVE_GETOPT)

if(NOT HAVE_GETOPT)
	# To find and use linenoise, as the vcpkg build does not have cmake config
	find_path(GETOPT_INCLUDE_DIR getopt.h)
	find_library(GETOPT_LIBRARY getopt)
	include_directories(${GETOPT_INCLUDE_DIR})
	target_link_libraries(${PROJECT_NAME} PUBLIC ${GETOPT_LIBRARY})
endif (NOT HAVE_GETOPT)

# See https://gitlab.kitware.com/cmake/community/wikis/doc/cpack/Packaging-With-CPack
INCLUDE(InstallRequiredSystemLibraries)

SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "CLD: A Docker Command Line Client")
SET(CPACK_PACKAGE_VENDOR "Abhishek Mishra <abhishekmishra3@gmail.com>")
SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
SET(CPACK_PACKAGE_VERSION_MAJOR "0")
SET(CPACK_PACKAGE_VERSION_MINOR "1")
SET(CPACK_PACKAGE_VERSION_PATCH "0")
SET(CPACK_PACKAGE_INSTALL_DIRECTORY "CLD")
FILE(GLOB CLD_DEPS_LICENSE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.*")
INSTALL(FILES ${CPACK_PACKAGE_DESCRIPTION_FILE} DESTINATION .)
INSTALL(FILES ${CPACK_RESOURCE_FILE_LICENSE} DESTINATION .)
INSTALL(FILES ${CLD_DEPS_LICENSE_FILES} DESTINATION .)
IF(WIN32 AND NOT UNIX)
  # There is a bug in NSI that does not handle full unix paths properly. Make
  # sure there is at least one set of four (4) backlasshes.
  #SET(CPACK_PACKAGE_ICON "${CMake_SOURCE_DIR}/Utilities/Release\\\\InstallIcon.bmp")
  FILE(GLOB CLD_BINARY_FILES "${CMAKE_CURRENT_BINARY_DIR}/Debug/*")
  INSTALL(FILES ${CLD_BINARY_FILES} DESTINATION bin)
  SET(CPACK_NSIS_INSTALLED_ICON_NAME "Debug\\\\cld.exe")
  SET(CPACK_NSIS_DISPLAY_NAME "CLD")
  SET(CPACK_NSIS_HELP_LINK "https:\\\\\\\\github.com/abhishekmishra/cld")
  SET(CPACK_NSIS_URL_INFO_ABOUT "https:\\\\\\\\github.com/abhishekmishra/cld")
  SET(CPACK_NSIS_CONTACT "abhishekmishra3@gmail.com")
  SET(CPACK_NSIS_MODIFY_PATH ON)
  SET(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
ELSE(WIN32 AND NOT UNIX)
  SET(CPACK_STRIP_FILES "cld")
  SET(CPACK_SOURCE_STRIP_FILES "")
ENDIF(WIN32 AND NOT UNIX)
SET(CPACK_PACKAGE_EXECUTABLES "cld" "cld")
INCLUDE(CPack)
