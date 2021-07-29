find_library(LZ4_LIBRARY NAMES lz4 PATH_SUFFIXES lib)
find_library(LZ4_LIBRARY_DEBUG NAMES lz4d PATH_SUFFIXES debug/lib)
find_path(LZ4_INCLUDE_DIRS NAMES lz4.h)

if(NOT LZ4_LIBRARY_DEBUG)
  set(LZ4_LIBRARY_DEBUG ${LZ4_LIBRARY})
endif(NOT LZ4_LIBRARY_DEBUG)

set(LZ4_LIBRARIES optimized ${LZ4_LIBRARY} debug ${LZ4_LIBRARY_DEBUG})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LZ4
  DEFAULT_MSG
  LZ4_LIBRARIES
  LZ4_INCLUDE_DIRS)
