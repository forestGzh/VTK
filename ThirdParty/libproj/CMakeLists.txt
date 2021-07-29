vtk_module_third_party(LibPROJ
  LIBRARIES vtklibproj
  USE_VARIABLES LibPROJ_MAJOR_VERSION
  INCLUDE_DIRS ${vtklibproj_SOURCE_DIR}/vtklibproj
               ${vtklibproj_BINARY_DIR}/vtklibproj)

if (NOT VTK_MODULE_USE_EXTERNAL_VTK_libproj)
  set(LibPROJ_MAJOR_VERSION "4")
endif ()