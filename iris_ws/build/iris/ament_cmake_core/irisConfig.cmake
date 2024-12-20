# generated from ament/cmake/core/templates/nameConfig.cmake.in

# prevent multiple inclusion
if(_iris_CONFIG_INCLUDED)
  # ensure to keep the found flag the same
  if(NOT DEFINED iris_FOUND)
    # explicitly set it to FALSE, otherwise CMake will set it to TRUE
    set(iris_FOUND FALSE)
  elseif(NOT iris_FOUND)
    # use separate condition to avoid uninitialized variable warning
    set(iris_FOUND FALSE)
  endif()
  return()
endif()
set(_iris_CONFIG_INCLUDED TRUE)

# output package information
if(NOT iris_FIND_QUIETLY)
  message(STATUS "Found iris: 0.0.0 (${iris_DIR})")
endif()

# warn when using a deprecated package
if(NOT "" STREQUAL "")
  set(_msg "Package 'iris' is deprecated")
  # append custom deprecation text if available
  if(NOT "" STREQUAL "TRUE")
    set(_msg "${_msg} ()")
  endif()
  # optionally quiet the deprecation message
  if(NOT ${iris_DEPRECATED_QUIET})
    message(DEPRECATION "${_msg}")
  endif()
endif()

# flag package as ament-based to distinguish it after being find_package()-ed
set(iris_FOUND_AMENT_PACKAGE TRUE)

# include all config extra files
set(_extras "")
foreach(_extra ${_extras})
  include("${iris_DIR}/${_extra}")
endforeach()
