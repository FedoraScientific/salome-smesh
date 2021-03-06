# - Find TBB
# Sets the following variables:
#   TBB_INCLUDE_DIRS - path to the TBB include directory
#   TBB_LIBRARIES    - path to the TBB libraries to be linked against
#

#########################################################################
# Copyright (C) 2007-2014  CEA/DEN, EDF R&D, OPEN CASCADE
#
# Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
# CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#

# ------

MESSAGE(STATUS "Check for TBB ...")

# ------

SET(TBB_ROOT_DIR $ENV{TBB_ROOT_DIR})

IF(TBB_ROOT_DIR)
  LIST(APPEND CMAKE_PREFIX_PATH "${TBB_ROOT_DIR}")
ENDIF(TBB_ROOT_DIR)

FIND_PATH(TBB_INCLUDE_DIRS tbb/tbb.h PATH_SUFFIXES include)

SET(TBBKERNEL cc4.1.0_libc2.4_kernel2.6.16.21)

IF(MACHINE_IS_64)
  SET(LIB_SUFFIX lib/intel64/${TBBKERNEL})
ELSE(MACHINE_IS_64)
  SET(LIB_SUFFIX lib/ia32/${TBBKERNEL})
ENDIF(MACHINE_IS_64)

FIND_LIBRARY(TBB_LIBRARY_tbb NAMES tbb PATH_SUFFIXES "${LIB_SUFFIX}")
FIND_LIBRARY(TBB_LIBRATY_tbbmalloc NAMES tbbmalloc PATH_SUFFIXES "${LIB_SUFFIX}")

SET(TBB_LIBRARIES
  ${TBB_LIBRARY_tbb}
  ${TBB_LIBRATY_tbbmalloc}
)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(TBB REQUIRED_VARS TBB_INCLUDE_DIRS TBB_LIBRARY_tbb TBB_LIBRATY_tbbmalloc)
