
find_library(LZMA_LIBRARY_RELEASE NAMES lzma PATH_SUFFIXES lib)
find_library(LZMA_LIBRARY_DEBUG NAMES lzmad PATH_SUFFIXES debug/lib)
find_path(LZMA_INCLUDE_DIRS NAMES lzma.h)

select_library_configurations(LZMA)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LZMA
  DEFAULT_MSG
  LZMA_LIBRARIES
  LZMA_INCLUDE_DIRS)
