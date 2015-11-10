# Copyright (c)      2014 Thomas Heller
# Copyright (c) 2007-2012 Hartmut Kaiser
# Copyright (c) 2010-2011 Matt Anderson
# Copyright (c) 2011      Bryce Lelbach
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

find_package(PkgConfig)
pkg_check_modules(PC_IBVERBS QUIET cci)

find_path(CCI_INCLUDE_DIR cci.h
  HINTS
  ${CCI_ROOT} ENV CCI_ROOT
  ${PC_CCI_INCLUDEDIR}
  ${PC_CCI_INCLUDE_DIRS}
  PATH_SUFFIXES include)

find_library(CCI_LIBRARY NAMES cci libcci
  HINTS
    ${CCI_ROOT} ENV CCI_ROOT
    ${PC_CCI_LIBDIR}
    ${PC_CCI_LIBRARY_DIRS}
  PATH_SUFFIXES lib lib64)

find_library(OPA_LIBRARY NAMES opa libopa
  HINTS
    ${CCI_ROOT} ENV CCI_ROOT
    ${PC_CCI_LIBDIR}
    ${PC_CCI_LIBRARY_DIRS}
  PATH_SUFFIXES lib lib64)

set(CCI_LIBRARIES ${CCI_LIBRARY} ${OPA_LIBRARY} CACHE INTERNAL "")
set(CCI_INCLUDE_DIRS ${CCI_INCLUDE_DIR} CACHE INTERNAL "")

find_package_handle_standard_args(CCI DEFAULT_MSG
  CCI_LIBRARY OPA_LIBRARY CCI_INCLUDE_DIR)

foreach(v CCI_ROOT)
  get_property(_type CACHE ${v} PROPERTY TYPE)
  if(_type)
    set_property(CACHE ${v} PROPERTY ADVANCED 1)
    if("x${_type}" STREQUAL "xUNINITIALIZED")
      set_property(CACHE ${v} PROPERTY TYPE PATH)
    endif()
  endif()
endforeach()

mark_as_advanced(CCO_ROOT OPA_LIBRARY CCI_LIBRARY CCI_INCLUDE_DIR)
