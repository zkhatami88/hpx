# Copyright (c) 2007-2013 Hartmut Kaiser
# Copyright (c) 2014-2015 Thomas Heller
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

include(HPX_AddLibrary)

################################################################################
# Decide whether to use the MPI based parcelport
################################################################################
if(HPX_WITH_PARCELPORT_CCI)
  find_package(CCI)
  if(NOT CCI_FOUND)
    hpx_error("The cci libraries could not be found and HPX_WITH_PARCELPORT_CCI=ON, please specify CCI_ROOT to point to the correct location or set HPX_PARCELPORT_CCI to OFF")
  endif()


  hpx_add_config_define(HPX_HAVE_PARCELPORT_CCI)
#   hpx_add_config_define(HPX_HAVE_PARCELPORT_IBVERBS_IFNAME "\"${HPX_WITH_PARCELPORT_IBVERBS_IFNAME}\"")
#   hpx_add_config_define(HPX_HAVE_PARCELPORT_IBVERBS_MESSAGE_PAYLOAD "${HPX_WITH_PARCELPORT_IBVERBS_MESSAGE_PAYLOAD}")
#   hpx_add_config_define(HPX_HAVE_PARCELPORT_IBVERBS_MEMORY_CHUNK_SIZE "${HPX_WITH_PARCELPORT_IBVERBS_MEMORY_CHUNK_SIZE}")
#   hpx_add_config_define(HPX_HAVE_PARCELPORT_IBVERBS_MAX_MEMORY_CHUNKS "${HPX_WITH_PARCELPORT_IBVERBS_MAX_MEMORY_CHUNKS}")

  macro(add_parcelport_cci_module)
    hpx_debug("add_parcelport_cci_module")
    include_directories(${CCI_INCLUDE_DIR})
    add_parcelport(cci
        STATIC
        SOURCES "${PROJECT_SOURCE_DIR}/plugins/parcelport/cci/parcelport_cci.cpp"
        DEPENDENCIES
              ${CCI_LIBRARIES}
              FOLDER "Core/Plugins/Parcelport/CCI"
        )
  endmacro()
else()
  macro(add_parcelport_cci_module)
  endmacro()
endif()
